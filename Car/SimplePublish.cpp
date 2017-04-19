#include "SimplePublish.h"

AmqpClient::SimplePublisher::SimplePublisher(Channel::ptr_t & channel, const string & publisher_name, const string & publisher_type)
	:m_channel(channel),m_publisherExchange(publisher_name),m_publisherType(publisher_type){
	/*if (m_publisherExchange == "") {
		m_publisherExchange = "amq.direct";
		boost::uuids::random_generator uuid_gen;
		boost::uuids::uuid guid(uuid_gen());
		m_publisherExchange += boost::lexical_cast<string>(guid);
	}*/
	m_channel->DeclareExchange(m_publisherExchange, m_publisherType);
}

AmqpClient::SimplePublisher::~SimplePublisher(){
	
}

void AmqpClient::SimplePublisher::Publish(const string &topic, const string & message){
	BasicMessage::ptr_t outgoing_message = BasicMessage::Create();
	outgoing_message->Body(message);
	m_channel->BasicPublish(m_publisherExchange, topic, outgoing_message);
}

void AmqpClient::SimplePublisher::Publish(const string &topic, const BasicMessage::ptr_t & message){
	m_channel->BasicPublish(m_publisherExchange, topic, message);
}
