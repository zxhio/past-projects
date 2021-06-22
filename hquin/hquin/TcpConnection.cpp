// File:    TcpConnection.cpp
// Author:  definezxh@163.com
// Date:    2019/06/29 13:51:43
// Desc:
//

#include <TcpConnection.h>
#include <Socket.h>
#include <EventLoop.h>
#include <Channel.h>

#include <unistd.h>
#include <string.h>

#include <lim/Log.h>

using namespace std::placeholders;

namespace hquin {

TcpConnection::TcpConnection(EventLoop *loop, std::string name, int sockfd,
                             const InetAddress &peerAddr)
    : eventloop_(loop), name_(name), state_(kConnecting),
      channel_(std::make_unique<Channel>(eventloop_, sockfd)),
      socket_(std::make_unique<Socket>(sockfd)), peerAddr_(peerAddr) {
    LOG_INFO << "new TcpConnection[" << name_ << "] "
             << peerAddr_.stringifyHost() << ", fd = " << sockfd
             << ", loop thread = " << eventloop_->threadId();
    channel_->setReadCallback(
        [&](Timestamp receiveTime) { handleRead(receiveTime); });
    channel_->setWriteCallback([&](int fd) { handleWrite(); });
    channel_->setCloseCallback([&](int fd) { handleClose(); });
    channel_->setErrorCallback([&](int fd) { handleError(); });
}

TcpConnection::~TcpConnection() {
    LOG_INFO << "TcpConnection closed [" << name_ << "] "
             << peerAddr_.stringifyHost() << ", fd = " << channel_->fd()
             << ", loop thread = " << eventloop_->threadId();
}

// send data to peer socket.
// try to send to socket fd, put the reamain data into output buffer, if
// write(2) not send completely.
void TcpConnection::send(const std::string &message) {
    // FIXME: multiple thread shoule do in loop.
    if (state_ == kConnected) {
        ssize_t nwrote =
            ::write(channel_->fd(), message.data(), message.size());
        if (nwrote >= 0) {
            if (static_cast<size_t>(nwrote) < message.size()) {
                LOG_INFO << "write data to fd is not complete.";
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOG_ERROR << "TcpConnection::sendInLoop";
            }
        }

        if (static_cast<size_t>(nwrote) < message.size()) {
            outputBuffer_.append(message.data() + nwrote);
            if (!channel_->isWriting()) {
                channel_->enableWriting();
            }
        }
    }
}

void TcpConnection::connectEstablished() {
    eventloop_->assertInLoopThread();
    setState(kConnected);
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

// half-close end of server writing.
void TcpConnection::shutdown() {
    if (state_ == kConnected) {
        setState(kDisconnecting);
        eventloop_->runInLoop([&]() { shutdownInLoop(); });
    }
}

void TcpConnection::shutdownInLoop() {
    if (!channel_->isWriting())
        socket_->shutdownWrite();
}

void TcpConnection::connectDestroyed() {
    eventloop_->assertInLoopThread();
    setState(kDisconnected);
    channel_->disableAll();
    eventloop_->removeChannel(channel_.get());
}

void TcpConnection::handleRead(Timestamp receiveTime) {
    eventloop_->assertInLoopThread();
    int saveErrno = 0;
    const int n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    } else if (n == 0) {
        handleClose(); // unreadble.
    } else {
        handleError();
    }
}

// write data to peer socket fd from output buffer.
// reduce fd use.
void TcpConnection::handleWrite() {
    eventloop_->assertInLoopThread();
    if (channel_->isWriting()) {
        ssize_t n = ::write(channel_->fd(), outputBuffer_.beginRead(),
                            outputBuffer_.readableBytes());
        if (n > 0) {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if (state_ == kDisconnecting)
                    shutdown();
            } else {
                LOG_INFO << "I am going to write more data";
            }
        } else {
            LOG_ERROR << "TcpConnection::handleWrite";
        }
    }
}

void TcpConnection::handleClose() {
    eventloop_->assertInLoopThread();
    assert(state_ == kConnected || state_ == kDisconnecting);
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError() { LOG_ERROR << strerror(errno); }

} // namespace hquin
