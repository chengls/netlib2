/*----------------------------------------------------------------
// Copyright (C) 2017 public
//
// 修改人：xiaoquanjie
// 时间：2017/11/11
//
// 修改人：xiaoquanjie
// 时间：
// 修改说明：
//
// 版本：V1.0.0
//----------------------------------------------------------------*/

#ifndef M_NETIO_NETIO_IMPL_INCLUDE
#define M_NETIO_NETIO_IMPL_INCLUDE

M_NETIO_NAMESPACE_BEGIN

NetIo::NetIo() :_backlog(20) {
}

NetIo::NetIo(SocketLib::s_uint32_t backlog) : _backlog(backlog) {
}

NetIo::~NetIo() {}

bool NetIo::ListenOne(const SocketLib::Tcp::EndPoint& ep) {
	try {
		NetIoTcpAcceptorPtr acceptor(new SocketLib::TcpAcceptor<SocketLib::IoService>(_ioservice, ep, _backlog));
		TcpSocketPtr clisock(new TcpSocket(*this, 0, 0));
		function_t<void(SocketLib::SocketError)> handler = bind_t(&NetIo::AcceptHandler, this, placeholder_1, clisock, acceptor);
		acceptor->AsyncAccept(handler, clisock->GetSocket());
	}
	catch (SocketLib::SocketError& error) {
		lasterror = error;
		return false;
	}
	return true;
}

void NetIo::Run() {

}

void NetIo::Stop() {
	try {
		_ioservice.Run();
	}
	catch (SocketLib::SocketError& error) {

	}
}

inline SocketLib::SocketError NetIo::GetLastError()const {
	return lasterror;
}

inline SocketLib::IoService& NetIo::GetIoService() {
	return _ioservice;
}

void NetIo::AcceptHandler(SocketLib::SocketError error, TcpSocketPtr clisock, NetIoTcpAcceptorPtr acceptor) {
	if (error) {
		M_NETIO_LOGGER("accept handler happend error:" << M_NETIO_LOGGER(error));
	}
	else {
		clisock->Init();
	}
	if (!_ioservice.Stopped()) {
		TcpSocketPtr clisock(new TcpSocket(*this, 0, 0));
		function_t<void(SocketLib::SocketError)> handler = bind_t(&NetIo::AcceptHandler, this, placeholder_1, clisock, acceptor);
		SocketLib::SocketError error2;
		acceptor->AsyncAccept(handler, clisock->GetSocket(),error2);
		if (error2)
			lasterror = error2;
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*以下三个函数定义为虚函数，以便根据实际业务的模式来做具体模式的消息包分发处理。
*保证同一个socket，以下三个函数的调用遵循OnConnected -> OnReceiveData -> OnDisconnected的顺序。
*保证同一个socket，以下后两个函数的调用都在同一个线程中
*/

// 连线通知,这个函数里不要处理业务，防止堵塞
void NetIo::OnConnected(TcpSocketPtr clisock) {

}

// 掉线通知,这个函数里不要处理业务，防止堵塞
void NetIo::OnDisconnected(TcpSocketPtr clisock) {

}

// 数据包通知,这个函数里不要处理业务，防止堵塞
void NetIo::OnReceiveData(TcpSocketPtr clisock, BufferPtr buffer) {

}

M_NETIO_NAMESPACE_END
#endif