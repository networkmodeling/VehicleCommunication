#include <iostream>
#include<SimpleAmqpClient\SimpleAmqpClient.h>

using namespace std;

namespace AmqpClient {
	class SIMPLEAMQPCLIENT_EXPORT SimplePublisher : boost::noncopyable {
	public:
		typedef boost::shared_ptr<SimplePublisher> ptr_t;
		//friend ptr_t boost::make_shared<SimplePublisher>(Channel::ptr_t &channel, const string &publisher_name);
		static ptr_t Create(Channel::ptr_t &channel, const string &publisher_name, const string &publisher_type) {
			return boost::make_shared<SimplePublisher>(channel, publisher_name, publisher_type);
		}
		explicit SimplePublisher(Channel::ptr_t &channel, const string &publisher_name, const string &publisher_type);
		virtual ~SimplePublisher();
		string getPublisherName() const {
			return m_publisherExchange;
		}
		void Publish(const string &queue,const string &message);
		void Publish(const string &queue,const BasicMessage::ptr_t & message);
	private:
		Channel::ptr_t m_channel;
		string m_publisherExchange;
		string m_publisherType;
	};
}


