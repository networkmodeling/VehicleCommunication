#include<iostream>
using namespace std;

class Info {
public:
	Info(string car_ID);
	Info(string car_ID, float velocity, float gap, float latitude, float longitude);
	Info(const Info &info);
	bool SetParamFromJson(string json_info);
	void SetVelocity(float velocity);
	void SetGap(float gap);
	void SetLocation(float latitude, float longitude);
	string GetCarId();
	float GetVelocity();
	float GetLocation();
	float GetGap();
	string GetString();
	string ToJason();
	~Info();
private:
	string car_ID;
	float velocity;
	float gap;
	struct Locale {
		float latitude;
		float longitude;
	} location;
};