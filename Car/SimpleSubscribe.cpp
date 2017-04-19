#include <iostream>
#include <SimpleAmqpClient\SimpleAmqpClient.h>
#include "SimpleSubscribe.h"

AmqpClient::SimpleSubscriber::SimpleSubscriber(Channel::ptr_t & channel, const string & queue_name, const string & exchange_name)
	:m_channel(channel), m_queue(queue_name){
	m_channel->DeclareQueue(m_queue);
	m_channel->BindQueue(m_queue, exchange_name, m_queue);
	m_consumerQueue = channel->BasicConsume(m_queue, "", true, true);
}

AmqpClient::SimpleSubscriber::~SimpleSubscriber(){
}

std::string AmqpClient::SimpleSubscriber::WaitForMessageString(int timeout){
	BasicMessage::ptr_t incoming = WaitForMessage(timeout);
	if (incoming != NULL)
		return incoming->Body();
	else
		return "";
}

AmqpClient::BasicMessage::ptr_t AmqpClient::SimpleSubscriber::WaitForMessage(int timeout){
	Envelope::ptr_t envelope;
	bool flag = m_channel->BasicConsumeMessage(m_consumerQueue, envelope, timeout);
	if (flag) { 
		return envelope->Message(); 
	}else{
		return NULL;
	} 
}
