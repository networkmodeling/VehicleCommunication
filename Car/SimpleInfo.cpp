#include "SimpleInfo.h"
#include "json\json.h"

Info::Info(string car_ID) {
	this->car_ID = car_ID;
	this->SetVelocity(0.0);
	this->SetGap(0.0);
	this->SetLocation(0.0, 0.0);
}

Info::Info(string car_ID, float velocity, float gap, float latitude, float longitude) {
	this->car_ID = car_ID;
	this->SetVelocity(velocity);
	this->SetGap(gap);
	this->SetLocation(latitude, longitude);
}

Info::Info(const Info & info){
	this->car_ID = info.car_ID;
	this->SetGap(info.gap);
	this->SetVelocity(info.velocity);
	this->SetLocation(info.location.latitude, info.location.longitude);
}

void Info::SetVelocity(float velocity) {
	this->velocity = velocity;
}

void Info::SetGap(float gap) {
	this->gap = gap;
}

void Info::SetLocation(float latitude, float longitude) {
	this->location.latitude = latitude;
	this->location.longitude = longitude;
}

string Info::GetCarId() {
	return this->car_ID;
}

float Info::GetVelocity() {
	return this->velocity;
}

float Info::GetLocation() {
	return this->location.latitude;
}

float Info::GetGap() {
	return this->gap;
}

string Info::GetString() {
	char outcome[200];
	int j = sprintf_s(outcome, 200, "%s,", this->car_ID.c_str());
	j += sprintf_s(outcome + j, 200 - j, "%f,", this->velocity);
	j += sprintf_s(outcome + j, 200 - j, "%f,", this->gap);
	j += sprintf_s(outcome + j, 200 - j, "%f,", this->location.latitude);
	j += sprintf_s(outcome + j, 200 - j, "%f", this->location.longitude);
	return string(outcome);
}

bool Info::SetParamFromJson(string json_info) {
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json_info, root, false)) {
		return false;
	}
	else {
		this->SetVelocity(root[0]["velocity"].asFloat());
		this->SetGap(root[0]["gap"].asFloat());
		this->SetLocation(root[0]["latitude"].asFloat(), root[0]["longitude"].asFloat());
		return true;
	}
}

string Info::ToJason() {
	Json::FastWriter writer;
	Json::Value root;
	Json::Value info;

	info["longitude"] = this->location.longitude;
	info["latitude"] = this->location.latitude;
	info["velocity"] = this->velocity;
	info["gap"] = this->gap;
	info["carid"] = this->car_ID;
	root.append(info);

	std::string json_file = writer.write(root);
	return json_file;
}

Info::~Info() {
}
