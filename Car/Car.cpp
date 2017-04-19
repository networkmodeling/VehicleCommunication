#include "SimplePublish.h"
#include "SimpleSubscribe.h"
#include "SimpleInfo.h"

#include<iostream>
#include<fstream>
#include<thread>
#include<Windows.h>
#include<SimpleAmqpClient\SimpleAmqpClient.h>
#include<json\json.h>

using namespace std;
using namespace AmqpClient;

extern string ip = "localhost";
extern int port = 5672;
extern string username = "guest";
extern string password = "guest";

void subscribe(Info *info, string EXCHANGE_NAME, string QUEUE_NAME) {
	//start thread
	//create channel, ready to receive message from server
	Channel::ptr_t channel = Channel::Create(ip,port,username,password);
	boost::shared_ptr<SimpleSubscriber> cons = SimpleSubscriber::Create(channel, QUEUE_NAME, EXCHANGE_NAME);
	while (1) {
		cons->WaitForMessageString(-1);
		string message = cons->WaitForMessageString(-1);
		if (message != ""){
			Json::Reader reader;
			Json::Value root;
			//if the string is json string, set the info
			if (reader.parse(message, root, false)) {
				float location = (*info).GetLocation();
				float v = (*info).GetVelocity();
				(*info).SetParamFromJson(message);							//set new param according to truck
				(*info).SetLocation(location + 0.1*v, 0);
				cout << (*info).GetString() << endl;
				//cout << message << endl;
			}else if (message == "join failure") {
				(*info).SetVelocity(0.0);
				(*info).SetLocation(0.0,0.0);
			}
		}
	}
}

void sender(Info *info, string EXCHANGE_NAME, string QUEUE_NAME) {
	ofstream ofile;
	ofile.open((*info).GetCarId()+"trajectory.csv", ios::app);
	Channel::ptr_t channel = Channel::Create(ip,port,username,password);
	boost::shared_ptr<SimplePublisher> pub = SimplePublisher::Create(channel, EXCHANGE_NAME, "topic");
	while(1){
		Sleep(1000);
		float location = (*info).GetLocation();
		(*info).SetLocation(location + (0.1*(*info).GetVelocity()), 0);	//set new location according to data
		pub->Publish(QUEUE_NAME, (*info).ToJason());
		cout << " [x] Sent " + (*info).GetString()<< endl;
		ofile << time(NULL) << "," << (*info).GetString() << "\n";
	}
	ofile.close();
}

void main(int arg,char * argv[]) {
	if (arg < 2) {
		cout << "no parameter error!" << endl;
	}
	string car_name = argv[1];
	string exchange_name = "exchange";
	string truck_name = "receive";
	ifstream ifile;
	ifile.open("information_c.txt", ios::in);
	if (!ifile) {
		cout << "È±ÉÙÅäÖÃÎÄ¼þ" << endl;
		return;
	}
	else {
		istreambuf_iterator<char> beg(ifile), end;
		string strdata(beg, end);
		ifile.close();
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(strdata, root, false)) {
			ip = root[0]["ip"].asString();
			cout << "ip:" << ip << endl;
			port = root[0]["port"].asInt();
			username = root[0]["username"].asString();
			password = root[0]["password"].asString();
			exchange_name = root[0]["exchange"].asString();
			truck_name = root[0]["truck"].asString();
		}
		Info info = Info(car_name, 0.0, 0.0, 0.0, 0.0);
		thread t_1(subscribe, &info, exchange_name, car_name);
		cout << "start subscribe thread" << endl;
		thread t_2(sender, &info, exchange_name, truck_name);
		cout << "start send thread" << endl;
		t_1.join();
		t_2.join();
	}
}