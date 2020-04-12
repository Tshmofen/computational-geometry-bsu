#include "geometry.h"
#include <cmath>
#include <iostream>
#include <ctime>
#include <algorithm>

#include <string>
#include <time.h>
#include <iomanip>

int previousSecond = 1;

void log(std::string logMessage) {
	struct tm newtime;
	time_t now = time(0);
	localtime_s(&newtime, &now);
	int h = newtime.tm_hour, m = newtime.tm_min, s = newtime.tm_sec;
	//if (s != previousSecond)
		std::cout << "[debug](" << h << ":" << m << ":" << s << "): " << logMessage.c_str() << "\n";
	previousSecond = s;
}

using std::vector;

//| a b |
//| c d | quadratic determinant
double qDet(double a, double b, double c, double d) {
	return a * d - b * c;
}
//generate some points with hundredths below the limit. also generate special "speed"
vector<Point> generatePoints(int number, int limit, bool isSpeed) {
	vector<Point> gPoints(number);
	srand(time(0));
	if (isSpeed)
		for (int i = 0; i < number; i++) {
			gPoints[i].x = (rand() % 200 / (double)100) - 1;
			gPoints[i].y = sqrt(1 - gPoints[i].x * gPoints[i].x); //|v| = 1
			if (rand() % 2 == 0)
				gPoints[i].y *= -1;
		}
	else
		for (int i = 0; i < number; i++) {
			gPoints[i].x = rand() % (2 * limit) + rand() % 100 / (double)100 - limit;
			if (gPoints[i].x == 0)
				gPoints[i].x += 0.01;
			gPoints[i].y = rand() % (2 * limit) + rand() % 100 / (double)100 - limit;
			if (gPoints[i].y == 0)
				gPoints[i].y += 0.01;
		}
	return gPoints;
}
vector<Line> makePolygonLines(vector<Point> verts) {
	vector<Line> lines(verts.size());
	for (int i = 0; i < lines.size(); i++) {
		if (i == lines.size() - 1)
			lines[i] = Line(verts[i], verts[0]);
		else
			lines[i] = Line(verts[i], verts[i + 1]);
	}
	return lines;
}
bool isSimplePolygon(vector<Point> verts) {
	vector<Line> lines = makePolygonLines(verts);
	bool is_intersect;
	for (int i = 0; i < lines.size(); i++) { //�������� ����������� ������ ������� � ������
		for (int j = i + 1; j < lines.size(); j++) {
			is_intersect = lines[i].isIntersects(lines[j]);
			if (is_intersect) //���������� ����� ��� ���� �� ����� �����������
				break;
		}
		if (is_intersect)
			break;
	}

	return is_intersect;
}
double getMinY(vector<Point> points) {
	double minY = points[0].y;
	for (int i = 1; i < points.size(); i++)
		minY = (points[i].y < minY) ? points[i].y : minY;
	return minY;
}
double getMinX(vector<Point> points) {
	double minX = points[0].x;
	for (int i = 1; i < points.size(); i++)
		minX = (points[i].x < minX) ? points[i].x : minX;
	return minX;
}
void removeRepeatings(vector<Point> &points) {
	for (int i = 0; i < points.size(); i++) {
		for (int j = i + 1; j < points.size(); j++) {
			if (points[i] == points[j]) {
				points.erase(points.begin() + j);
				j--;
			}
		}
	}
}
double cos(Point vector1, Point vector2) {
	return (vector1 * vector2) / (vector1.getLength() * vector2.getLength());
}
bool isInClockWise(Point a, Point b, Point c) {
	return a.x*(b.y - c.y) + b.x*(c.y - a.y) + c.x*(a.y - b.y) < 0;
} //is three points right
bool isInCounterClockWise(Point a, Point b, Point c) {
	return a.x*(b.y - c.y) + b.x*(c.y - a.y) + c.x*(a.y - b.y) > 0;
} //is three points left
void stretchAndMove(vector<Point> &points, Point newOrigin, double stretch) {
	for (int i = 0; i < points.size(); i++) {
		points[i].x = stretch * (points[i].x - newOrigin.x);
		points[i].y = stretch * (points[i].y - newOrigin.y);
	}
}
int getOctane(Point p1, Point p2) {
	double x = p2.x - p1.x;
	double y = p2.y - p1.y;

	if (0 <= y && y <= x)
		return 1;
	else if (0 < x && x <= y)
		return 2;
	else if (0 <= -x && -x < y)
		return 3;
	else if (0 < y && y <= -x)
		return 4;
	else if (0 <= -y && -y < -x)
		return 5;
	else if (0 < -x && -x <= -y)
		return 6;
	else if (0 < x && x < -y)
		return 7;
	else if (0 < -y && -y <= -x)
		return 8;
	else
		return 1;
}

//function to sort the points array for buildConvex;
bool compare(Point a, Point b) {
	return a.x < b.x || a.x == b.x && a.y < b.y;
}
vector<Point> buildConvexHull(vector<Point> points) {
	sort(points.begin(), points.end(), &compare); //sorting points to buildHullFunction
	vector<Point> up = { points[0] },
		down = { points[0] };

	for (int i = 0; i < points.size(); i++) {
		Point p1 = points[0], p2 = points.back();
		if (i == points.size() - 1 || isInClockWise(p1, points[i], p2)) { //making upper part of shell
			while (up.size() >= 2 && !isInClockWise(up[up.size() - 2], up[up.size() - 1], points[i]))
				up.pop_back();
			up.push_back(points[i]);
		}
		if (i == points.size() - 1 || isInCounterClockWise(p1, points[i], p2)) { //making lower part of shell
			while (down.size() >= 2 && !isInCounterClockWise(down[down.size() - 2], down[down.size() - 1], points[i]))
				down.pop_back();
			down.push_back(points[i]);
		}
	}

	vector<Point> convexHull;
	for (int i = 0; i < up.size(); i++)
		convexHull.push_back(up[i]);
	for (int i = down.size() - 2; i > 0; i--)
		convexHull.push_back(down[i]);
	return convexHull;
}
double triangleArea(Point p1, Point p2, Point p3) {
	Point v1 = p2 - p1, 
		v2 = p3 - p2,
		v3 = p3 - p1;
	double s_perim = (v1.getLength() + v2.getLength() + v3.getLength()) / 2;
	return sqrt(abs(s_perim * (s_perim - v1.getLength()) * (s_perim - v2.getLength()) * s_perim - v3.getLength()));
}
double getDiameter(vector<Point> points) {
	int p_index = points.size() - 1, q_index = 0;
	int n = points.size() - 1;
	
	while (triangleArea(points[p_index % n], points[(p_index + 1) % n], points[(q_index + 1) % n]) > 
		triangleArea(points[p_index % n], points[(p_index + 1) % n], points[q_index % n]))
		q_index += 1;

	int q0_index = q_index;
	double d = 0;
	int i = 0;
    while (points[q_index % n] != points[0] && i < n){
		p_index += 1;
		i++;
		if (i > 100)
			break;
        while (triangleArea(points[p_index % n], points[(p_index + 1) % n], points[(q_index + 1) % n]) > 
                triangleArea(points[p_index % n], points[(p_index + 1) % n], points[q_index % n])) {
			q_index += 1;
			if ((points[p_index % n] - points[q_index % n]).getLength() != (points[q0_index % n] - points[0]).getLength())
				if ((points[p_index % n] - points[q_index % n]).getLength() >= d)
					d = (points[p_index % n] - points[q_index % n]).getLength();
		}
        if (triangleArea(points[p_index % n], points[(p_index + 1) % n], points[(q_index + 1) % n]) == 
                triangleArea(points[p_index % n], points[(p_index + 1) % n], points[q_index % n])) {
			if ((points[p_index % n] - points[q_index % n]).getLength() != (points[q0_index % n] - points[-1]).getLength())
				if ((points[p_index % n] - points[q_index % n]).getLength() >= d)
					d = (points[p_index % n] - points[q_index % n]).getLength();
		}
	}
	return d;
}
vector<Point> get2PointsDiameter(vector<Point> points) {//return two points!
	for (int i = 0; i < points.size(); i++) {
		for (int j = 0; j < points.size(); j++) {
			if ((points[i] - points[j]).getLength() == getDiameter(points))
				return { points[i], points[j] };
		}
	}
	return { points[0], points[points.size()/2] };
}

bool Point::isInPolygon(vector<Point> verts) {
	vector<Line> lines = makePolygonLines(verts);
	Point pt = *this;
	double x_max = verts[0].x, y_max = verts[0].y
		, x_min = verts[0].x, y_min = verts[0].y
		, x_max_ind  = 0;
	for (int i = 1; i < verts.size(); i++) { //���������� �������� ������
		if (verts[i].x > x_max) {
			x_max = verts[i].x;
			x_max_ind;
		}
		if (verts[i].x < x_min)
			x_min = verts[i].x;
		if (verts[i].y > y_max)
			y_max = verts[i].y;
		if (verts[i].y < y_min)
			y_min = verts[i].y;
	}
	if (pt.x > x_max || pt.x < x_min || pt.y > y_max || pt.y < y_min) { //���������� ����
		return false;
	}

	if (lines[x_max_ind].isPointRight(pt)) {
		return false;
	}
	x_max_ind = (x_max_ind == 0) ? lines.size() - 1 : x_max_ind - 1; //���������� ���������� �������
	if (lines[x_max_ind].isPointRight(pt)) {
		return false;
	}

	for (int i = 0; i < lines.size(); i++) {
		if (lines[i].havePoint(pt)) //���� ����� ��������� �� ������� ������
			return true;
	}

	Line checking_ray(pt, {x_max + 1, pt.y});
	int num_of_intersections = 0;
	for (int i = 0; i < lines.size(); i++) {
		if (checking_ray.isIntersects(lines[i], false)) {//���� ������������ ����� ������������ � ����� ������
			if (checking_ray.havePoint(lines[i].A)) {//�������� �� ����������� ����� ����� ����� ���� ������
				int previous_ind = (i == 0) ? lines.size() - 1 : i - 1; // ���������� �������
				if (lines[i].isOnOneLine(lines[previous_ind])) {
					num_of_intersections--;//�� ��������� ����� ������
				}
			}
			num_of_intersections++;
		}
	}

	/*cout << "\n\n------\n" "Number of intesections - \"" << num_of_intersections //debug information
		<< "\"\nMax/Min values (xmax, xmin, ymax, ymin): " << x_max << ", "
		<< x_min << ", " << y_max << ", " << y_min << ";\n------\n\n";*/

	if (num_of_intersections % 2 == 0)
		return false;
	return true;
}
bool Point::inMinMaxBorderOfPolygon(vector<Point> lines) {
	double xMin = lines[0].x, xMax = lines[0].x
		, yMin = lines[0].y, yMax = lines[0].y;
	for (int i = 1; i < lines.size(); i++) {
		xMin = (lines[i].x < xMin) ? lines[i].x : xMin;
		xMax = (lines[i].x > xMax) ? lines[i].x : xMax;
		yMin = (lines[i].y < yMin) ? lines[i].y : yMin;

		yMax = (lines[i].y > yMax) ? lines[i].y : yMax;
	}
	if (x < xMin || x > xMax || y < yMin || y > yMax)
		return false;
	return true;
}
//Sector test for convex polygon gicen by clockwise
bool Point::isInSectorOfConvex(vector<Point> verts) {
	vector<Line> lines = makePolygonLines(verts);
	if (lines[0].isPointRight(*this) || lines[lines.size() - 1].isPointRight(*this)) {
		return false;
	}

	int start = 0, end = verts.size() - 1;
	while (end - start > 1) {
		int sep = floor((start + end) / 2);
		Line ln_temp = { verts[0] , verts[sep] };
		if (ln_temp.isPointLeft(*this)) {
			start = sep;
		}
		else {
			end = sep;
		}
	}

	Line ln_temp = { verts[start], verts[start + 1] };
	if (ln_temp.isPointLeft(*this)) {
		return true;
	}
	return false;
}
//������� ����
bool Point::isInSimplePolygon(vector<Point> verts) {
	int s = 0;
	for (int i = 0; i < verts.size() - 1; i++) {
		int beta1 = getOctane(*this, verts[i]);
		int beta2 = getOctane(*this, verts[i + 1]);
		int delta = beta2 - beta1;
		if (delta > 4)
			delta -= 8;
		if (delta < -4)
			delta += 8;
		if (delta == 4 || delta == -4) {
			double d = qDet(verts[i].x - x, verts[i].y - y, verts[i + 1].x - x, verts[i + 1].y - y);
			if (d > 0)
				delta = 4;
			if (d < 0)
				delta = -4;
			if (d == 0)
				return true;
		}
		s += delta;
	}
	//std::cout << "s = " << s << "\n\n";
	if (s == 8 || s == -8)
		return true;
	return false;
}
double Point::getLength() {
	return sqrt(*this * *this);
}

//line segment intersection check
bool Line::isIntersects(Line ln, bool toCheckParallel) {
	double x1 = A.x
		, x2 = B.x
		, x3 = ln.A.x
		, x4 = ln.B.x
		, y1 = A.y
		, y2 = B.y
		, y3 = ln.A.y
		, y4 = ln.B.y;
	double d1 = qDet(x4 - x3, y4 - y3, x3 - x1, y3 - y1) //������� �������������
		, d2 = qDet(x4 - x3, y4 - y3, x3 - x2, y3 - y2)
		, d3 = qDet(x2 - x1, y2 - y1, x3 - x1, y3 - y1)
		, d4 = qDet(x2 - x1, y2 - y1, x4 - x1, y4 - y1);

	if (d1*d2 < 0 && d3*d4 < 0)
		return true;

	if (toCheckParallel && d1 == d2 && d1 == d3 && d1 == d4 && d1 == 0) { //������������ ������ ���������� �� ����� ������
		double c1 = (x3 - x1)*(x4 - x1) + (y3 - y1)*(y4 - y1) //��������� ������������
			, c2 = (x3 - x2)*(x4 - x2) + (y3 - y2)*(y4 - y2)
			, c3 = (x1 - x3)*(x2 - x3) + (y1 - y3)*(y2 - y3)
			, c4 = (x1 - x4)*(x2 - x4) + (y1 - y4)*(y2 - y4);
		if (c1 < 0 || c2 < 0 || c3 < 0 || c4 < 0)
			return true;
	}

	if (!toCheckParallel && (this->havePoint(ln.A) || this->havePoint(ln.B)
		|| ln.havePoint(A) || ln.havePoint(B))) {
		return true;
	}

	return false;
}
//Checks if point lies on line segment
bool Line::havePoint(Point pt) {
	double x1 = A.x, y1 = A.y
		, x2 = B.x, y2 = B.y
		, x_max = (x1 > x2) ? x1 : x2
		, y_max = (y1 > y2) ? y1 : y2
		, x_min = (x1 < x2) ? x1 : x2
		, y_min = (y1 < y2) ? y1 : y2;
	if (pt.x < x_min || pt.x > x_max || pt.y < y_min || pt.y > y_max)
		return false;
	//����� ����� ��������� ������ Ax + By + C = 0.
	double eA = y2 - y1
		, eB = x1 - x2
		, eC = x2 * y1 - x1 * y2;
	if (eA*pt.x + eB * pt.y + eC == 0)
		return true;
	return false;
}
//Checks if two line segments lies on one line
bool Line::isOnOneLine(Line ln) {
	double x1 = A.x, y1 = A.y
		, x2 = B.x, y2 = B.y;
	//����� ��������� ������ Ax + By + C = 0.
	double eA = y2 - y1
		, eB = x1 - x2
		, eC = x2 * y1 - x1 * y2;
	if (eA * ln.A.x + eB * ln.A.y + eC == 0 && eA * ln.B.x + eB * ln.B.y + eC == 0) {//��� ����� ������� �������
		return true;
	}
	return false;
}
//checks if point intersect line segment with current speed on the next tick
bool Line::willBeIntersectedBy(Point pt, Point speed) {
	double det, detNew, x = pt.x, y = pt.y; 
	double x1 = A.x, x2 = B.x, y1 = A.y, y2 = B.y; 
	det = qDet(x2 - x1, y2 - y1, x - x1, y - y1);    //if (det <= 0) ����� "������" ��� "��"
	detNew = qDet(x2 - x1, y2 - y1, x + speed.x - x1, y + speed.x - y1); //if (det >= 0) ����� "�����" ��� "��"
	if (det >= 0 && detNew <= 0 || det <= 0 && detNew >= 0)
		return true;
	return false;
}
bool Line::isPointLeft(Point pt) {
	if(qDet(B.x - A.x, B.y - A.y, pt.x - A.x, pt.y - A.y) < 0) 
		return true;
	return false;
}
bool Line::isPointRight(Point pt) {
	if (qDet(B.x - A.x, B.y - A.y, pt.x - A.x, pt.y - A.y) > 0)
		return true;
	return false;
}