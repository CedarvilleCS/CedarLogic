
#pragma once

struct Point {

	Point();

	Point(float x, float y);



	Point operator+(const Point &other) const;

	Point operator-(const Point &other) const;

	void operator+=(const Point &other);

	void operator-=(const Point &other);

	bool operator==(const Point &other) const;

	bool operator!=(const Point &other) const;



	float x;

	float y;

	static constexpr float EqualRange = 0.00125f;
};