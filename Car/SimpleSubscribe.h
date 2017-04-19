#include <iostream>
#include<SimpleAmqpClient\SimpleAmqpClient.h>
using namespace std;
namespace AmqpClient{
	class SIMPLEAMQPCLIENT_EXPORT SimpleSubscriber
	{
	public:
		typedef boost::shared_ptr<SimpleSubscriber> ptr_t;
		//friend ptr_t boost::make_shared<SimpleSubscriber>(AmqpClient::Channel::ptr_t const &channel, string const &queue_name);
		static ptr_t Create(AmqpClient::Channel::ptr_t channel, const string &queue_name, const string &exchange_name=""){
			return boost::make_shared<SimpleSubscriber>(channel, queue_name, exchange_name);
		}
		explicit SimpleSubscriber(Channel::ptr_t &channel, const string &queue_name, const string &exchange_name="");
	public:
		virtual ~SimpleSubscriber();
		string SimpleSubscriber::WaitForMessageString(int timeout);
		BasicMessage::ptr_t SimpleSubscriber::WaitForMessage(int timeout);
	private:
		Channel::ptr_t m_channel;
		string m_queue;
		string m_consumerQueue;
	};
}
