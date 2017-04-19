#include"SimplePublish.h"
#include"SimpleSubscribe.h"
#include"SimpleInfo.h"

#include<iostream>
#include<thread>
#include<fstream>
#include<ctime>
#include<vector>
#include<Windows.h>
#include<SimpleAmqpClient\SimpleAmqpClient.h>
#include<json\json.h>

using namespace std;
using namespace AmqpClient;

extern string ip = "localhost";
extern int port = 5672;
extern string username = "guest";
extern string password = "guest";
extern int l = 5;

void publish(string EXCHANGE_NAME, vector<Info> * carInfo) {
	float delta = 1;
	//start thread
	ofstream ofile;
	ofile.open((*carInfo).front().GetCarId()+"trajectory.csv", ios::app);
	//int num_car = *carInfo.size();
	vector<Info> info_n;
	//create channel, ready to send messages to clients
	Channel::ptr_t channel = Channel::Create(ip,port,username,password);
	boost::shared_ptr<SimplePublisher> pub = SimplePublisher::Create(channel, EXCHANGE_NAME, "topic");
	while (1) {
		Sleep(1000);
		int car_num = (*carInfo).size();
		if (car_num > 1) {
			if ((int)info_n.size() < car_num - 1) {
				for (int i = info_n.size() + 1; i < car_num; i++) {
					info_n.push_back(Info((*carInfo)[i]));
				}
			}
			////////////////////////     compute gap & speed     //////////////////////// 
			for (int i = 1; i < (int)(*carInfo).size(); i++) {
				if (i == 2) {
					string a = "";
				}
				float l0 = (*carInfo)[i-1].GetLocation();
				float l1 = (*carInfo)[i].GetLocation();
				float v0 = (*carInfo)[i - 1].GetVelocity();
				float v1 = (*carInfo)[i].GetVelocity();
				// the distance is unsafe according to the 2-second rule
				if (l1 > (l0-l)) {
					cout << "crash";
					break;
				}
				// change the latter car speed according to the former car 
				if ((l0 - l1) < (2 * v1)) {//unsafe, slow down
					info_n[i - 1].SetVelocity(v0 > delta ? v0 - delta : 0);		//if the speed is below delta, then stop the car
				}
				if ((l0 - l1) >= (2 * v1)) {//safe, accelarate
					info_n[i - 1].SetVelocity(v1 + delta);			//if the distance is too long, then add speed
				}
				info_n[i - 1].SetGap(l0 - l1 - l);
			}
			for (unsigned int i = 0; i < info_n.size(); i++) {
				pub->Publish((*carInfo)[i + 1].GetCarId(), info_n[i].ToJason());
				cout << "send message to:" << info_n[i].GetString() << endl;
			}
		}
		else {
			cout << "no car" << endl;
		}
		////////////////////////        set the truck location     ////////////////////////
		(*carInfo).front().SetLocation((*carInfo).front().GetLocation() + (0.1*(*carInfo).front().GetVelocity()), 0);
		//////////////////////// *        vary the truck speed     //////////////////////// 
		srand((int)time(NULL));
		int vary = 2 * (rand() % 2) - 1;						//determin increase or decrease
		float varied_v = (rand() / (double)RAND_MAX) * delta;		//determin the value of increase of decrease within 5m/s
		float current_v = (*carInfo).front().GetVelocity();				//get the current speed of truck
		float new_v = (current_v < 33) ? (current_v + vary*varied_v) : (current_v - varied_v); //set the new speed of truck
		(*carInfo).front().SetVelocity(new_v > 0 ? new_v : current_v);		//ensure the speed is above zero
		ofile << (*carInfo).front().GetString() << endl;					//record the speed of truck
	}
}

void receiver(string EXCHANGE_NAME, vector<Info> * carInfo) {
	//start thread
	//open csv file, ready to record
	vector<string> car_names;
	car_names.push_back((*carInfo).front().GetCarId());
	ofstream ofile;
	ofile.open("trajectory.csv", ios::app);
	//create channel, ready to receive messages from clients
	Channel::ptr_t cl = Channel::Create(ip, port, username, password);
	Channel::ptr_t channel = Channel::Create(ip,port, username, password);
	string QUEUE_NAME = (*carInfo).front().GetCarId();
	boost::shared_ptr<SimplePublisher> cl_cons = SimplePublisher::Create(cl, EXCHANGE_NAME, "topic");
	boost::shared_ptr<SimpleSubscriber> cons = SimpleSubscriber::Create(channel, QUEUE_NAME, EXCHANGE_NAME);
	while (1) {
		string info = cons->WaitForMessageString(100);
		if (info != "") {
			Json::Reader reader;
			Json::Value root;
			//if the string is json string, set the alpha_info and beta_info according the json string
			if (reader.parse(info, root, false)) {
				string carid = root[0]["carid"].asString();
				vector<string>::iterator iter = find(car_names.begin(), car_names.end(), carid);
				if (iter == car_names.end()){
					float l0 = (*carInfo).back().GetLocation(); //location of former car
					float v1 = root[0]["velocity"].asFloat();
					if ((l0 < (2 * v1))||(l0 < l)) {
						cout << carid+" car join failure" << endl;
						cl_cons->Publish(carid, "join failure");
					}else{
						(*carInfo).push_back(Info(carid));
						car_names.push_back(carid);
						(*carInfo).back().SetParamFromJson(info);
						cout << "add new car" << endl;
						//cout << "receive:" << (*carInfo).back().GetString() << endl;
						ofile << time(NULL) << "," << (*carInfo).back().GetString() << "\n";
					}
				}else {
					int dist = distance(car_names.begin(), iter);
					(*carInfo)[dist].SetParamFromJson(info);
					//cout << "receive:" << ((Info)*iter).GetString() << endl;
					ofile << time(NULL) << "," << (*carInfo)[dist].GetString() << "\n";
				}				
			}
		}
	}
	ofile.close();
}

void main() {
	ifstream ifile;
	string exchange_name = "exchange";
	string truck_name = "receive";
	ifile.open("information_t.txt",ios::in);
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
			//ip = "localhost";
			cout << "ip:" << ip << endl;
			port = root[0]["port"].asInt();
			username = root[0]["username"].asString();
			password = root[0]["password"].asString();
			exchange_name = root[0]["exchange"].asString();
			truck_name = root[0]["truck"].asString();
		}
		vector<Info> car;
		car.push_back(Info(truck_name));
		car.front().SetVelocity(26.8224); // initialize the velocity of truck (m/s)
		thread t_1(publish, exchange_name, &car);
		cout << "start publish thread" << endl;
		thread t_2(receiver, exchange_name, &car);
		cout << "start receive thread" << endl;
		t_1.join();
		t_2.join();	
	}
}
