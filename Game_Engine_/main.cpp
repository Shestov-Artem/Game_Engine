#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

#include <vector>
#include <algorithm>

using namespace std;

int obj_count = 0;  //������� �������� � �����
int wall_count = 0; //������� ����

double radius = 200; //������ ���������

SDL_Renderer* renderer = nullptr;

SDL_Texture* LoadImage(std::string file) {
	SDL_Surface* loadedImage = nullptr;
	SDL_Texture* texture = nullptr;
	loadedImage = IMG_Load(file.c_str());
	if (loadedImage != nullptr) {
		texture = SDL_CreateTextureFromSurface(renderer, loadedImage);
		SDL_FreeSurface(loadedImage);
	}
	else
		std::cout << SDL_GetError() << std::endl;
	return texture;
}

class ObjectWithShadow2 {

private:

	// ������� ��� ���������� �����, ������� �� �������� �������� �� �������, �������� ����� �������
	SDL_Point findThirdPoint(SDL_Rect dest1, SDL_Point p, double distance) {
		// ��������� ������ �����������
		double dx = p.x - dest1.x;
		double dy = p.y - dest1.y;

		// ��������� ����� �������
		double length = std::sqrt(dx * dx + dy * dy);

		// ����������� ������ �����������
		double unit_dx = dx / length;
		double unit_dy = dy / length;

		// ��������� ���������� ������� �����
		SDL_Point p3;
		p3.x = p.x + distance * unit_dx;
		p3.y = p.y + distance * unit_dy;

		return p3;
	}

	// ������� ��� ���������� ���������� ������������ ���� ��������
	int dotProduct(const SDL_Point& A, const SDL_Point& B) {
		return A.x * B.x + A.y * B.y;
	}

	// ������� ��� ���������� ����� �������
	double magnitude(const SDL_Point& A) {
		return std::sqrt(A.x * A.x + A.y * A.y);
	}

	// ������� ��� ���������� ���� ����� ����� ��������� � ��������
	double angleBetweenVectors(const SDL_Point& A, const SDL_Point& B) {
		int dot = dotProduct(A, B);
		double magA = magnitude(A);
		double magB = magnitude(B);

		// ���������� �������� ����
		double cosTheta = dot / (magA * magB);

		// ����������� �������� �������� ��� ��������� ������ ��-�� �������������� �����������
		cosTheta = std::max(-1.0, std::min(1.0, cosTheta));

		// ���������� ���� � ��������
		return std::acos(cosTheta);
	}

	//��������� �������� ��������� ���� ����� � ���� ����� ����
	struct TwoPoints_and_angle {
		double angle;
		int x1; int y1;
		int x2; int y2;
	};

public:

	int x_1; int y_1;
	int x_2; int y_2;
	int x_3; int y_3;
	int x_4; int y_4;

	int x_1_base; int y_1_base;
	int x_2_base; int y_2_base;
	int x_3_base; int y_3_base;
	int x_4_base; int y_4_base;

	//����������� �� ���������
	ObjectWithShadow2() {}

	//�����������
	ObjectWithShadow2(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
		x_1 = x1;
		y_1 = y1;
		x_2 = x2;
		y_2 = y2;
		x_3 = x3;
		y_3 = y3;
		x_4 = x4;
		y_4 = y4;

		x_1_base = x1; y_1_base = y1;
		x_2_base = x2; y_2_base = y2;
		x_3_base = x3; y_3_base = y3;
		x_4_base = x4; y_4_base = y4;

		obj_count++;
	}

	void move_obj(SDL_Rect back_dest) {
		x_1 = back_dest.x + x_1_base;
		y_1 = back_dest.y + y_1_base;
		x_2 = back_dest.x + x_2_base;
		y_2 = back_dest.y + y_2_base;
		x_3 = back_dest.x + x_3_base;
		y_3 = back_dest.y + y_3_base;
		x_4 = back_dest.x + x_4_base;
		y_4 = back_dest.y + y_4_base;
	}

	void drawObject() {
		SDL_RenderDrawLine(renderer, x_1, y_1, x_2, y_2);
		SDL_RenderDrawLine(renderer, x_2, y_2, x_3, y_3);
		SDL_RenderDrawLine(renderer, x_3, y_3, x_4, y_4);
		SDL_RenderDrawLine(renderer, x_4, y_4, x_1, y_1);
	}

	//������� ����������� ���������� �� ��������� ����� �� �������
	int Find_min_dist(SDL_Rect dest1) {
		int min_dist;

		int distans[4];
		//������� ���������� �� ���� ������� ����� �������
		distans[0] = sqrt((x_1 - dest1.x) * (x_1 - dest1.x) + (y_1 - dest1.y) * (y_1 - dest1.y));
		distans[1] = sqrt((x_2 - dest1.x) * (x_2 - dest1.x) + (y_2 - dest1.y) * (y_2 - dest1.y));
		distans[2] = sqrt((x_3 - dest1.x) * (x_3 - dest1.x) + (y_3 - dest1.y) * (y_3 - dest1.y));
		distans[3] = sqrt((x_4 - dest1.x) * (x_4 - dest1.x) + (y_4 - dest1.y) * (y_4 - dest1.y));

		//������� ����������� ����������
		min_dist = distans[0];
		for (int i = 0; i < 4; i++) {
			if (distans[i] < min_dist) min_dist = distans[i];
		}

		return min_dist;
	}

	//�������� ���������� ������ ����������� ����� � ������� 
	void GetPosPointObj(SDL_Rect dest1, SDL_Point& obj_pos_1, SDL_Point& obj_pos_2) {

		SDL_Point vectorA = { x_1 - dest1.x, y_1 - dest1.y };
		SDL_Point vectorB = { x_2 - dest1.x, y_2 - dest1.y };
		SDL_Point vectorC = { x_3 - dest1.x, y_3 - dest1.y };
		SDL_Point vectorD = { x_4 - dest1.x, y_4 - dest1.y };

		TwoPoints_and_angle p1;
		TwoPoints_and_angle p2;
		TwoPoints_and_angle p3;
		TwoPoints_and_angle p4;
		TwoPoints_and_angle p5;
		TwoPoints_and_angle p6;

		// ������� ���� ����� ���������
		p1.angle = angleBetweenVectors(vectorA, vectorB);
		p2.angle = angleBetweenVectors(vectorA, vectorC);
		p3.angle = angleBetweenVectors(vectorA, vectorD);
		p4.angle = angleBetweenVectors(vectorB, vectorC);
		p5.angle = angleBetweenVectors(vectorB, vectorD);
		p6.angle = angleBetweenVectors(vectorC, vectorD);

		//��������� ���������� �����, ����� �������� ��� ���� ��������
		p1.x1 = x_1; p1.y1 = y_1; p1.x2 = x_2; p1.y2 = y_2;
		p2.x1 = x_1; p2.y1 = y_1; p2.x2 = x_3; p2.y2 = y_3;
		p3.x1 = x_1; p3.y1 = y_1; p3.x2 = x_4; p3.y2 = y_4;
		p4.x1 = x_2; p4.y1 = y_2; p4.x2 = x_3; p4.y2 = y_3;
		p5.x1 = x_2; p5.y1 = y_2; p5.x2 = x_4; p5.y2 = y_4;
		p6.x1 = x_3; p6.y1 = y_3; p6.x2 = x_4; p6.y2 = y_4;

		TwoPoints_and_angle angles[6] = { p1, p2, p3, p4, p5, p6 };

		//������� ������������ ���� ����� ���������
		TwoPoints_and_angle max_angle = angles[0];
		for (int i = 0; i < 6; i++)
			if (angles[i].angle > max_angle.angle) max_angle = angles[i];

		//������ ��� ���� ���� �� ���� ����� ������� ������
		obj_pos_1.x = max_angle.x1;
		obj_pos_1.y = max_angle.y1;
		obj_pos_2.x = max_angle.x2;
		obj_pos_2.y = max_angle.y2;
	}

	//�������� ���������� ������ ����������� �������� ����� � ����������� ��������� 
	void GetCirclePointsPos2(SDL_Point& circle_pos_1, SDL_Point& circle_pos_2, SDL_Rect dest1, SDL_Point obj_pos_1, SDL_Point obj_pos_2) {

		//������� ���������� �� ������� ����� �� ������ ������� ����� ������� � ����� 
		int distans1 = sqrt((obj_pos_1.x - dest1.x) * (obj_pos_1.x - dest1.x) + (obj_pos_1.y - dest1.y) * (obj_pos_1.y - dest1.y));
		int distans2 = sqrt((obj_pos_2.x - dest1.x) * (obj_pos_2.x - dest1.x) + (obj_pos_2.y - dest1.y) * (obj_pos_2.y - dest1.y));

		//������� ����� �����, ������� �� ��� �� �������, �� �� ������ ��� ����������
		circle_pos_1 = findThirdPoint(dest1, obj_pos_1, radius - distans1);
		circle_pos_2 = findThirdPoint(dest1, obj_pos_2, radius - distans2);
	}

	//������� ����� �� ������� ����� �������� 4-� ��������
	SDL_Point OptimizationDrawShadow2(SDL_Rect dest1, SDL_Point obj_pos_1, SDL_Point obj_pos_2, SDL_Point circle_pos_1, SDL_Point circle_pos_2, int i) {

		SDL_Point points2;

		// ����� ������ ��� ����� ����� ������ ������� � �����
		if (i == 0) points2 = { obj_pos_1.x, obj_pos_1.y };
		if (i == 1) points2 = { obj_pos_2.x, obj_pos_2.y };

		// ���� ��� ���������� ����� ����� ������ ������� ��������� (��� ��������� -> ��������� ��� ����)
		if (sqrt((dest1.x - obj_pos_1.x) * (dest1.x - obj_pos_1.x) + (dest1.y - obj_pos_1.y) * (dest1.y - obj_pos_1.y)) < radius &&
			sqrt((dest1.x - obj_pos_2.x) * (dest1.x - obj_pos_2.x) + (dest1.y - obj_pos_2.y) * (dest1.y - obj_pos_2.y)) < radius) {
			if (i == 2) points2 = { circle_pos_2.x, circle_pos_2.y };
			if (i == 3) points2 = { circle_pos_1.x, circle_pos_1.y };
		}
		// ���� ���� ����� ����� ��� ������� ��������� (������ �� ������������ �� ���� �������)
		else if (sqrt((dest1.x - obj_pos_1.x) * (dest1.x - obj_pos_1.x) + (dest1.y - obj_pos_1.y) * (dest1.y - obj_pos_1.y)) < radius) {
			if (i == 2) points2 = { obj_pos_2.x, obj_pos_2.y };
			if (i == 3) points2 = { circle_pos_1.x, circle_pos_1.y };
		}
		// ���� ������ ����� ����� ��� ������� ��������� (������ �� ������������ �� ���� �������)
		else if (sqrt((dest1.x - obj_pos_2.x) * (dest1.x - obj_pos_2.x) + (dest1.y - obj_pos_2.y) * (dest1.y - obj_pos_2.y)) < radius) {
			if (i == 2) points2 = { circle_pos_2.x, circle_pos_2.y };
			if (i == 3) points2 = { obj_pos_1.x, obj_pos_1.y };
		}
		// ���� ��� ����� ����� ��� ������� ���������
		else {
			if (i == 2) points2 = { obj_pos_2.x, obj_pos_2.y };
			if (i == 3) points2 = { obj_pos_1.x, obj_pos_1.y };
		}
		return points2;
	}

	//������������ �������������� (���������������)
	void fillConvexPolygon(SDL_Renderer* renderer, SDL_Point points[], int count) {
		// ������� ������� ��������������
		int minX = points[0].x, maxX = points[0].x;
		int minY = points[0].y, maxY = points[0].y;

		for (int i = 1; i < count; ++i) {
			if (points[i].x < minX) minX = points[i].x;
			if (points[i].x > maxX) maxX = points[i].x;
			if (points[i].y < minY) minY = points[i].y;
			if (points[i].y > maxY) maxY = points[i].y;
		}

		// ������������� ���� ��� �������
		//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // ������ ����

		// ������ ��� �������� �����������
		std::vector<std::pair<int, int>> intersections;

		// ������������ ������ ������ �� minY �� maxY
		for (int y = minY; y <= maxY; ++y) {
			intersections.clear();

			// ������� ����������� � ������� ��������������
			for (int i = 0; i < count; ++i) {
				int j = (i + 1) % count; // ��������� �������
				if ((points[i].y <= y && points[j].y > y) || (points[i].y > y && points[j].y <= y)) {
					// ��������� x-���������� �����������
					int x = points[i].x + (y - points[i].y) * (points[j].x - points[i].x) / (points[j].y - points[i].y);
					intersections.emplace_back(x, y);
				}
			}

			// ��������� ����������� �� x
			std::sort(intersections.begin(), intersections.end());

			// ��������� ������� ����� ������ �����������
			for (size_t i = 0; i < intersections.size(); i += 2) {
				if (i + 1 < intersections.size()) {
					int xStart = intersections[i].first;
					int xEnd = intersections[i + 1].first;

					// ������ ����� ����� xStart � xEnd �� ������� ������ y
					SDL_RenderDrawLine(renderer, xStart, y, xEnd, y);
				}
			}
		}
	}

	//����������� ������ ������ ����������, ������������ ������� (���������������)
	void fillCircleSegment(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Point point1, SDL_Point point2) {

		// ������������� ���� ��� �������
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // ����� ����

		// ��������� ���� � ��������
		double angle1 = atan2(point1.y - centerY, point1.x - centerX);
		double angle2 = atan2(point2.y - centerY, point2.x - centerX);

		// ��������, ��� angle1 < angle2
		if (angle1 > angle2) {
			std::swap(angle1, angle2);
		}

		// ������� ��� ����� ������ ����������
		for (int y = centerY - radius; y <= centerY + radius; y++) {
			int x_min = centerX;   //����������� �� �-�������� ��������, ��������������� ���� �������� 
			int x_max = centerX;   //������������ �� �-�������� ��������, ��������������� ���� �������� 
			bool one_x_min = true; //����� ����� ������ ��������
			int Y = centerY;       //�������� �� y-��������, ��������������� ���� �������� 
			for (int x = centerX - radius; x <= centerX + radius; x++) {
				if ((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) <= radius * radius) {
					// ������� ��� ����� ������������ ������ � �����

					double angle = atan2(y - centerY, x - centerX);   //���� ����� ������ (x,y) � ������� ����������

					//����������� ������ ����� ���� �� ������ ������� �� ������ �������� �� ���� ���������� (����� ������ angle2 = angle2, � ��� angle1 = angle1 + 360)
					if (angle2 - angle1 > 3.1416) {
						//������������ �����
						if (angle >= angle2) {
							//������������ ������
							int D = (point2.x - point1.x) * (y - point1.y) - (point2.y - point1.y) * (x - point1.x);
							double angle3 = atan2(point1.y - centerY, point1.x - centerX);
							double angle4 = atan2(point2.y - centerY, point2.x - centerX);
							if (D > 0) {
								if (angle3 < angle4) {
									//������� ������ � ��������� ����, � ����� ������ �������� y
									if (one_x_min) {
										x_min = x;
										Y = y;
										one_x_min = false;
									}
									x_max = x;
								}
							}
							else if (D <= 0) {
								if (angle3 > angle4) {
									if (one_x_min) {
										x_min = x;
										Y = y;
										one_x_min = false;
									}
									x_max = x;
								}
							}
						}
						//������������ �����
						//if (angle <= angle1) {
						else {
							//������������ ������
							int D = (point2.x - point1.x) * (y - point1.y) - (point2.y - point1.y) * (x - point1.x);
							double angle3 = atan2(point1.y - centerY, point1.x - centerX);
							double angle4 = atan2(point2.y - centerY, point2.x - centerX);
							if (D > 0) {
								if (angle3 < angle4) {
									if (one_x_min) {
										x_min = x;
										Y = y;
										one_x_min = false;
									}
									x_max = x;
								}
							}
							else if (D <= 0) {
								if (angle3 > angle4) {
									if (one_x_min) {
										x_min = x;
										Y = y;
										one_x_min = false;
									}
									x_max = x;
								}
							}
						}
					}
					else {
						//������������ �����
						if (angle >= angle1 && angle <= angle2) {
							//������������ ������
							int D = (point2.x - point1.x) * (y - point1.y) - (point2.y - point1.y) * (x - point1.x);
							double angle3 = atan2(point1.y - centerY, point1.x - centerX);
							double angle4 = atan2(point2.y - centerY, point2.x - centerX);
							if (D > 0) {
								if (angle3 > angle4) {
									if (one_x_min) {
										x_min = x;
										Y = y;
										one_x_min = false;
									}
									x_max = x;
								}
							}
							else if (D <= 0) {
								if (angle3 < angle4) {
									if (one_x_min) {
										x_min = x;
										Y = y;
										one_x_min = false;
									}
									x_max = x;
								}
							}
						}
					}
				}
			}
			SDL_RenderDrawLine(renderer, x_min, Y, x_max, Y);
		}
	}
};

class Wall : public ObjectWithShadow2 {
private:
public:

	int height;

	Wall() {}

	//�����������
	Wall(int x1, int y1, int x2, int y2, int h) {
		x_1 = x1;
		y_1 = y1;
		x_2 = x2;
		y_2 = y2;
		x_3 = x2;
		y_3 = y2 - h;
		x_4 = x1;
		y_4 = y1 - h;

		x_1_base = x1; y_1_base = y1;
		x_2_base = x2; y_2_base = y2;
		x_3_base = x2; y_3_base = y2 - h;
		x_4_base = x1; y_4_base = y1 - h;

		height = h;

		wall_count++;
	}

	void move_wall(SDL_Rect back_dest) {
		x_1 = back_dest.x + x_1_base;
		y_1 = back_dest.y + y_1_base;
		x_2 = back_dest.x + x_2_base;
		y_2 = back_dest.y + y_2_base;
		x_3 = back_dest.x + x_3_base;
		y_3 = back_dest.y + y_3_base;
		x_4 = back_dest.x + x_4_base;
		y_4 = back_dest.y + y_4_base;
	}

	//����� ����� ����������� ���� ��������
	bool doIntersect(SDL_Point p1, SDL_Point q1, SDL_Point p2, SDL_Point q2, SDL_Point& intersection) {
		double a1 = q1.y - p1.y;
		double b1 = p1.x - q1.x;
		double c1 = a1 * p1.x + b1 * p1.y;

		double a2 = q2.y - p2.y;
		double b2 = p2.x - q2.x;
		double c2 = a2 * p2.x + b2 * p2.y;

		double determinant = a1 * b2 - a2 * b1;

		if (determinant == 0) {
			return false; // ����� �����������
		}
		else {
			intersection.x = (b2 * c1 - b1 * c2) / determinant;
			intersection.y = (a1 * c2 - a2 * c1) / determinant;

			// ��������, ��������� �� ����� ����������� �� ����� ��������
			bool onSegment1 = (std::min(p1.x, q1.x) <= intersection.x && intersection.x <= std::max(p1.x, q1.x) &&
				std::min(p1.y, q1.y) <= intersection.y && intersection.y <= std::max(p1.y, q1.y));
			bool onSegment2 = (std::min(p2.x, q2.x) <= intersection.x && intersection.x <= std::max(p2.x, q2.x) &&
				std::min(p2.y, q2.y) <= intersection.y && intersection.y <= std::max(p2.y, q2.y));

			return onSegment1 && onSegment2; // ���������� true, ���� ����� ����������� �� ����� ��������
		}
	}

	//������ ���� � ������� ����� ����� ����� ���������� �����
	void CleanWall() {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_Point points[4] = { {x_1, y_1}, {x_1, y_1 - height}, {x_2, y_2 - height}, {x_2, y_2} };
		fillConvexPolygon(renderer, points, 4);
	}

	//��� ����� � ������
	void DarkWall() {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_Point points[4] = { {x_1, y_1}, {x_1, y_1 - height}, {x_2, y_2 - height}, {x_2, y_2} };
		fillConvexPolygon(renderer, points, 4);
	}

	//������ ����� ����������� ���� AB � ������� CD
	SDL_Point findIntersection(SDL_Point A, SDL_Point B, SDL_Point C, SDL_Point D) {
		// ������������ ��� ���������
		double a1 = B.y - A.y; // y2 - y1
		double b1 = A.x - B.x; // x1 - x2
		double c1 = a1 * A.x + b1 * A.y; // a1*x + b1*y = c1

		double a2 = D.y - C.y; // y4 - y3
		double b2 = C.x - D.x; // x3 - x4
		double c2 = a2 * C.x + b2 * C.y; // a2*x + b2*y = c2

		double determinant = a1 * b2 - a2 * b1;

		// ���� ����������� ����� ����, ����� �����������
		if (determinant == 0) {
			return { 0, 0 }; // ��� �����������
		}

		// ������� ����� �����������
		int x = (b2 * c1 - b1 * c2) / determinant;
		int y = (a1 * c2 - a2 * c1) / determinant;

		// ���������, ��������� �� ����� ����������� �� ������� CD
		if (x < std::min(C.x, D.x) || x > std::max(C.x, D.x) || y < std::min(C.y, D.y) || y > std::max(C.y, D.y)) {
			return { 0, 0 }; // ����� ����������� �� ����� �� �������
		}

		//���������, ����� �� ��������� ����� �� ����������� �������
		double dxA = B.x - A.x;
		double dyA = B.y - A.y;
		// ������ AP
		double dxAP = x - A.x;
		double dyAP = y - A.y;
		// ��������� ������������
		double dotProduct = dxA * dxAP + dyA * dyAP;
		// ���������, ����� �� ����� �� �������
		if (dotProduct > 0) {
			return SDL_Point{ x, y }; // ����� P ����� �� ����������� �������
		}
		else {
			return { 0, 0 }; // ����� P ����� � ��������������� �����������
		}
	}

	//������ ����� �� ������� ����� �������� ���� �� ����� (���� ��� �� �����) 
	SDL_Point GetShadowPoint(SDL_Rect dest1, SDL_Point point) {
		SDL_Point A{ dest1.x, dest1.y };
		SDL_Point B{ point.x, point.y };
		SDL_Point C{ x_1, y_1 };
		SDL_Point D{ x_2, y_2 };

		return findIntersection(A, B, C, D);
	}

	//������ ����� �����������, ������� ������� ��� �� ����� ������ ���� (���� �� ����� ��� �����)
	SDL_Point GetShadowControlPoint_1(SDL_Rect dest1, SDL_Point point_1, SDL_Point point_2) {
		SDL_Point A{ dest1.x, dest1.y };
		SDL_Point B{ x_1, y_1 };
		SDL_Point C{ point_1.x, point_1.y };
		SDL_Point D{ point_2.x, point_2.y };

		return findIntersection(A, B, C, D);
	}
	SDL_Point GetShadowControlPoint_2(SDL_Rect dest1, SDL_Point point_1, SDL_Point point_2) {
		SDL_Point A{ dest1.x, dest1.y };
		SDL_Point B{ x_2, y_2 };
		SDL_Point C{ point_1.x, point_1.y };
		SDL_Point D{ point_2.x, point_2.y };

		return findIntersection(A, B, C, D);
	}

	//�������� ��� ����� ����������� ���� �������� ����� �� �������
	void GetTwoPointsWithPeresek(SDL_Rect dest1, SDL_Point point_1, SDL_Point point_2, SDL_Point& result_point_1, SDL_Point& result_point_2) {

		//������� ����� ����������� ������ ���� �� ������� (���� �� ������������, ���������� {0, 0})
		SDL_Point p1 = GetShadowPoint(dest1, point_1);
		SDL_Point p2 = GetShadowPoint(dest1, point_2);

		//���������� � ������ ������
		if (p1.x != 0 && p1.y != 0)
			result_point_1 = p1;
		if (p2.x != 0 && p2.y != 0) {
			if (result_point_1.x == 0 && result_point_1.y == 0) result_point_1 = p2;
			else result_point_2 = p2;
		}
	}

	//�� ���������� ���� ����� ������ ����
	void DrawShadowOnWell(SDL_Point p1, SDL_Point p2, SDL_Rect dest1, ObjectWithShadow2 object, bool if_shadow_on_wall_left, bool if_shadow_on_wall_right) {

		//�������������� ����� �� ���������� x
		if (p1.x > p2.x) {
			SDL_Point p3 = p2;
			p2 = p1;
			p1 = p3;
		}

		//������ ����
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		int h = height; //������ ����

		//���� ��� ����� ����� �� ������ ������
		if (p1.x >= x_1 && p1.x <= x_2 && p2.x >= x_1 && p2.x <= x_2) {
			SDL_Rect rect;
			rect.w = p2.x - p1.x;
			rect.h = h;
			rect.x = p1.x;
			rect.y = p1.y - rect.h;
			SDL_RenderFillRect(renderer, &rect);
		}

		//���� ������ ���� ����� �� ������ ������ 
		else if (p1.x <= x_1 && p2.x >= x_1 && p2.x <= x_2) {

			//���� ����� ������ ���� �����
			if (if_shadow_on_wall_left) {
				SDL_Rect rect;
				rect.w = p2.x - x_1;
				rect.h = h;
				rect.x = x_1;
				rect.y = y_1 - rect.h;
				SDL_RenderFillRect(renderer, &rect);
			}

			//���� ����� ����� ���� �����
			if (if_shadow_on_wall_right) {
				SDL_Rect rect;
				rect.w = x_2 - p2.x;
				rect.h = h;
				rect.x = p2.x;
				rect.y = y_2 - rect.h;
				SDL_RenderFillRect(renderer, &rect);
			}
		}

		//���� ����� ������ ����
		else if (if_shadow_on_wall_left && if_shadow_on_wall_right) {
			SDL_Rect rect;
			rect.w = x_2 - x_1;
			rect.h = h;
			rect.x = x_1;
			rect.y = y_1 - rect.h;
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	// ������� ��� ��������, ����� �� ����� (px, py) �� �������, �������� ������� (x1, y1) � (x2, y2)
	bool isPointOnSegment(SDL_Point p, SDL_Point a, SDL_Point b) {
		int crossProduct = (p.y - a.y) * (b.x - a.x) - (p.x - a.x) * (b.y - a.y);

		if (crossProduct > 200 || crossProduct < -200) {
			return false; // ����� �� ����������� �������
		}

		if (std::min(a.x, b.x) <= p.x && p.x <= std::max(a.x, b.x) &&
			std::min(a.y, b.y) <= p.y && p.y <= std::max(a.y, b.y)) {
			return true; // ����� ����� �� �������
		}

		return false; // ����� �� ����� �� �������
	}

	//���������� ��������� �����
	void DrawWall() {
		SDL_RenderDrawLine(renderer, x_1, y_1, x_2, y_2);
		SDL_RenderDrawLine(renderer, x_2, y_2, x_2, y_2 - height);
		SDL_RenderDrawLine(renderer, x_2, y_2 - height, x_1, y_1 - height);
		SDL_RenderDrawLine(renderer, x_1, y_1 - height, x_1, y_1);
	}
};

//���������� �� ��������
void bubbleSort(ObjectWithShadow2 arr[], int dist[], int n) {
	for (int i = 0; i < n - 1; ++i) {
		for (int j = 0; j < n - i - 1; ++j) {
			if (dist[j] < dist[j + 1]) {
				// ����� ����������

				ObjectWithShadow2 temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;

				int temp2 = dist[j];
				dist[j] = dist[j + 1];
				dist[j + 1] = temp2;
			}
		}
	}
}

//��������� ����� �� ������
void DrawWallShadow(Wall walls[], ObjectWithShadow2 objects[], SDL_Rect dest1) {

	//�������� ��������� ������ ��� ���� ����� ��� ������ ����� ������� ����� ��� ��������� ����� �� ���
	SDL_Point** points_1_on_wall = new SDL_Point * [wall_count];
	for (int i = 0; i < wall_count; i++)
		points_1_on_wall[i] = new SDL_Point[obj_count];
	SDL_Point** points_2_on_wall = new SDL_Point * [wall_count];
	for (int i = 0; i < wall_count; i++)
		points_2_on_wall[i] = new SDL_Point[obj_count];
	//�������������
	for (int i = 0; i < wall_count; i++) {
		for (int j = 0; j < obj_count; j++) {
			points_1_on_wall[i][j] = { 0, 0 };
			points_2_on_wall[i][j] = { 0, 0 };
		}
	}

	//�������� ��������� ������ ��� ���� ����� ������, ������ �� ���� ��������� �����
	bool** if_shadow_on_wall_left = new bool* [wall_count];
	for (int i = 0; i < wall_count; i++)
		if_shadow_on_wall_left[i] = new bool[obj_count];
	bool** if_shadow_on_wall_right = new bool* [wall_count];
	for (int i = 0; i < wall_count; i++)
		if_shadow_on_wall_right[i] = new bool[obj_count];
	//�������������
	for (int i = 0; i < wall_count; i++) {
		for (int j = 0; j < obj_count; j++) {
			if_shadow_on_wall_left[i][j] = false;
			if_shadow_on_wall_right[i][j] = false;
		}
	}

	//��������� ������� �� �������� ���������� �� ���, � ����� ������� � ������ �������� ������������ ������� ��� ����, � ����� ��� ������
	int* dist_objects = new int[obj_count];
	//������ ����������� ����������
	for (int i = 0; i < obj_count; i++)
		dist_objects[i] = objects[i].Find_min_dist(dest1);
	//�����������
	bubbleSort(objects, dist_objects, obj_count);


	//������ ���� �� ��������
	for (int i = 0; i < obj_count; i++) {

		//������������ ���������� �� ��������� ����� �� ������� ����� �������
		int distans1 = sqrt((objects[i].x_1 - dest1.x) * (objects[i].x_1 - dest1.x) + (objects[i].y_1 - dest1.y) * (objects[i].y_1 - dest1.y));
		int distans2 = sqrt((objects[i].x_2 - dest1.x) * (objects[i].x_2 - dest1.x) + (objects[i].y_2 - dest1.y) * (objects[i].y_2 - dest1.y));
		int distans3 = sqrt((objects[i].x_3 - dest1.x) * (objects[i].x_3 - dest1.x) + (objects[i].y_3 - dest1.y) * (objects[i].y_3 - dest1.y));
		int distans4 = sqrt((objects[i].x_4 - dest1.x) * (objects[i].x_4 - dest1.x) + (objects[i].y_4 - dest1.y) * (objects[i].y_4 - dest1.y));
		//���� ������ ������ ����� �����
		if (distans1 <= radius || distans2 <= radius || distans3 <= radius || distans4 <= radius) {

			//�������� ���������� ������ ����������� ����� � ������� 
			SDL_Point obj_pos_1;
			SDL_Point obj_pos_2;
			objects[i].GetPosPointObj(dest1, obj_pos_1, obj_pos_2);

			//����� ����������� �������� ����� � ����������
			SDL_Point circle_pos_1;
			SDL_Point circle_pos_2;
			objects[i].GetCirclePointsPos2(circle_pos_1, circle_pos_2, dest1, obj_pos_1, obj_pos_2);

			//������� ����� 4-� ���������
			SDL_Point points[4];

			//������� ������ ��� ����� � ������ ���� ��� ���� ������ ���� ������ ������ ���������� (�� ����������� ��� ������������� ����� ���� � �������)
			for (int k = 0; k < 4; k++)
				points[k] = objects[i].OptimizationDrawShadow2(dest1, obj_pos_1, obj_pos_2, circle_pos_1, circle_pos_2, k);

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			//����� ������ 4-� �������� �� 4-� ������
			objects[i].fillConvexPolygon(renderer, points, 4);
			//����������� ������� ����������, ������������ ����� � ������
			objects[i].fillCircleSegment(renderer, dest1.x, dest1.y, radius, points[2], points[3]);

			//����� ��� ������ ����� ������ �� ����� �����
			for (int j = 0; j < wall_count; j++) {

				//���� �� ���� �����
				if (dest1.y > walls[j].y_1) {

					SDL_Point p1 = { 0 ,0 };
					SDL_Point p2 = { 0, 0 };
					walls[j].GetTwoPointsWithPeresek(dest1, points[2], points[3], p1, p2);

					//����� ��������� ��� �����, ����� �� ������� ��� ����� �� �����
					SDL_Point p0 = { dest1.x, dest1.y };

					if (!walls[j].isPointOnSegment(points[0], p0, p1) && !walls[j].isPointOnSegment(points[1], p0, p1)) { //���� �� ���� ����� ������� �� ����� �� ������� �� ��������� �� �����
						p1 = { 0, 0 };
					}
					if (!walls[j].isPointOnSegment(points[0], p0, p2) && !walls[j].isPointOnSegment(points[1], p0, p2)) {
						p2 = { 0, 0 };
					}

					//��������� ����� ����������� ����� ���� � ���������� �����
					points_1_on_wall[j][i] = p1;
					points_2_on_wall[j][i] = p2;

					//��������� true ���� ����� ��������� ������ ���� �� �������
					if (walls[j].GetShadowControlPoint_1(dest1, points[2], points[3]).y != 0)
						if_shadow_on_wall_left[j][i] = true;
					if (walls[j].GetShadowControlPoint_2(dest1, points[2], points[3]).y != 0)
						if_shadow_on_wall_right[j][i] = true;
				}
			}
		}
	}

	//������ ���� �� ������
	for (int i = 0; i < wall_count; i++) {
		walls[i].CleanWall();  //������ ����� �� �����

		if (dest1.y < walls[i].y_1)
			walls[i].DarkWall();

		for (int j = 0; j < obj_count; j++) {
			//����� ���� �� ������
			if (walls[i].y_1 < objects[j].y_4) { //���� ������ ���� �����
				walls[i].DrawShadowOnWell(points_1_on_wall[i][j], points_2_on_wall[i][j], dest1, objects[j], if_shadow_on_wall_left[i][j], if_shadow_on_wall_right[i][j]);
			}
		}
	}
}

//�������� ����� �������� ����
void ApplyingGeneralShadow(ObjectWithShadow2 objects[], SDL_Rect dest1, SDL_Rect dest_shadow, SDL_Texture* shadow, Wall walls[]) {
	// �������� �������� ��� ����������
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 1250, 650);
	// ��������� �������� ��� ������� ������
	SDL_SetRenderTarget(renderer, texture);
	// ������� ��������
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // ����� ����
	SDL_RenderClear(renderer);

	//����� ���� �� �������� (� ��� ����� � �� ������)
	DrawWallShadow(walls, objects, dest1);

	//������ ����� ����
	dest_shadow.x = dest1.x - 750 - 1;
	dest_shadow.y = dest1.y - 750;
	SDL_RenderCopy(renderer, shadow, NULL, &dest_shadow);

	// ������� � ��������� �������
	SDL_SetRenderTarget(renderer, NULL);
	// ��������� ������ ���������� ��� ��������
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_MUL);   //SDL_BLENDMODE_MUL: color multiply dstRGB = (srcRGB * dstRGB) + (dstRGB * (1 - srcA)), dstA = dstA
	// ������ ����� �������� ������������ �������� ��� ���������
	SDL_SetTextureAlphaMod(texture, 220); // ��������� �����-������
	// ��������� ������������ �������� �� ������
	SDL_Rect destRect = { 0, 0, 1250, 650 };
	SDL_RenderCopy(renderer, texture, NULL, &destRect);

	SDL_DestroyTexture(texture);
}

//����� ������������ ���������
class Character {

private:
	int speed = 5; //�������� ������������ ���������
	bool Moving;   //��������� �� ��������
	int Position;  //� ����� ������� ������� �������� (0 - ����, 1 - ����, 2 - �����, 3 - ������)
	int BinAnim;   //������� ������ (���� �� 12 ������)
	bool Anim;     //����� ��������
	int n;         //����� � ����� �������� �������� �������� ��� ���������� ������� ��������
	int old_mas[4] = { 0, 0, 0, 0 };  //����� ������� ���� ������ �� ���������� ����
	SDL_Texture* Pers_down_mov_1 = LoadImage("down_mov_1.png");
	SDL_Texture* Pers_down_mov_2 = LoadImage("down_mov_2.png");
	SDL_Texture* Pers_up_mov_1 = LoadImage("up_mov_1.png");
	SDL_Texture* Pers_up_mov_2 = LoadImage("up_mov_2.png");
	SDL_Texture* Pers_left_mov_1 = LoadImage("left_mov_1.png");
	SDL_Texture* Pers_left_mov_2 = LoadImage("left_mov_2.png");
	SDL_Texture* Pers_right_mov_1 = LoadImage("right_mov_1.png");
	SDL_Texture* Pers_right_mov_2 = LoadImage("right_mov_2.png");

	SDL_Texture* Pers_down_stay_1 = LoadImage("down_stay_1.png");
	SDL_Texture* Pers_down_stay_2 = LoadImage("down_stay_2.png");
	SDL_Texture* Pers_up_stay_1 = LoadImage("up_stay_1.png");
	SDL_Texture* Pers_up_stay_2 = LoadImage("up_stay_2.png");
	SDL_Texture* Pers_left_stay_1 = LoadImage("left_stay_1.png");
	SDL_Texture* Pers_left_stay_2 = LoadImage("left_stay_2.png");
	SDL_Texture* Pers_right_stay_1 = LoadImage("right_stay_1.png");
	SDL_Texture* Pers_right_stay_2 = LoadImage("right_stay_2.png");

public:

	SDL_Rect dest;

	//�����������
	Character(int x, int y) {
		dest.x = x;
		dest.y = y;
		SDL_QueryTexture(Pers_down_mov_1, NULL, NULL, &dest.w, &dest.h);
		dest.w /= 2;
		dest.h /= 2;

		Moving = false;
		Position = 0;  //� ����� ������� ������� �������� (0 - ����, 1 - ����, 2 - �����, 3 - ������)
		BinAnim = 0;
		Anim = false;  //����� ��������
		n = 0;
	}

	//�������� ������ ��� ��������� ���������� ��������
	void GetCorrectAnim() {

		if (n == 2) { Moving = true; Position = 1; }
		else if (n == 3) { Moving = true; Position = 2; }
		else if (n == 1) { Moving = true; Position = 0; }
		else if (n == 4) { Moving = true; Position = 3; }

		if (BinAnim < 11) BinAnim++;
		else BinAnim = 0;
		if (BinAnim == 0) Anim = true;
		else if (BinAnim == 6) Anim = false;
	}

	//������������ ������ ��������
	void CreateAnimations() {

		if (Anim) {  //�������� �1
			if (Moving) {
				if (Position == 0) SDL_RenderCopy(renderer, Pers_down_mov_1, NULL, &dest);  //�������� ����
				else if (Position == 1) SDL_RenderCopy(renderer, Pers_up_mov_1, NULL, &dest);  //�������� �����
				else if (Position == 2) SDL_RenderCopy(renderer, Pers_left_mov_1, NULL, &dest);  //�������� �����
				else if (Position == 3) SDL_RenderCopy(renderer, Pers_right_mov_1, NULL, &dest);  //�������� ������
			}
			else {
				if (Position == 0) SDL_RenderCopy(renderer, Pers_down_stay_1, NULL, &dest);  //���� ������ ����
				else if (Position == 1) SDL_RenderCopy(renderer, Pers_down_stay_1, NULL, &dest);  //���� ������ �����
				else if (Position == 2) SDL_RenderCopy(renderer, Pers_down_stay_1, NULL, &dest);  //���� ������ �����
				else if (Position == 3) SDL_RenderCopy(renderer, Pers_down_stay_1, NULL, &dest);  //���� ������ ������
			}
		}
		else {  //�������� �2
			if (Moving) {
				if (Position == 0) SDL_RenderCopy(renderer, Pers_down_mov_2, NULL, &dest);
				else if (Position == 1) SDL_RenderCopy(renderer, Pers_up_mov_2, NULL, &dest);
				else if (Position == 2) SDL_RenderCopy(renderer, Pers_left_mov_2, NULL, &dest);
				else if (Position == 3) SDL_RenderCopy(renderer, Pers_right_mov_2, NULL, &dest);
			}
			else {
				if (Position == 0) SDL_RenderCopy(renderer, Pers_down_stay_2, NULL, &dest);
				else if (Position == 1) SDL_RenderCopy(renderer, Pers_down_stay_2, NULL, &dest);
				else if (Position == 2) SDL_RenderCopy(renderer, Pers_down_stay_2, NULL, &dest);
				else if (Position == 3) SDL_RenderCopy(renderer, Pers_down_stay_2, NULL, &dest);
			}
		}

		Moving = false;
	}

	//������������ �� ������� �����
	void InteractionWithBorders(SDL_Renderer* renderer) {  //�������� ���� ������� � �������� �����������
		// �������� ������� ����
		int width, height;
		SDL_GetRendererOutputSize(renderer, &width, &height);

		int x = dest.x;
		int y = dest.y;

		// ������ ������� �� ���������
		Uint32* pixels = new Uint32[width * height];
		SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, pixels, width * sizeof(Uint32));

		Uint32 pixel = pixels[y * width + (x + dest.w / 2)];  // �������� ���� ������� �������
		Uint8 r, g, b, a; // ��������� RGBA ����������
		SDL_GetRGBA(pixel, SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), &r, &g, &b, &a);
		if (r == 255 && g == 0 && b == 0) dest.y += speed;

		pixel = pixels[(y + dest.h / 2) * width + x];  // �������� ���� ������� �������
		SDL_GetRGBA(pixel, SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), &r, &g, &b, &a);
		if (r == 255 && g == 0 && b == 0) dest.x += speed;

		pixel = pixels[(y + dest.h) * width + (x + dest.w / 2)];  // �������� ���� ������� �������
		SDL_GetRGBA(pixel, SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), &r, &g, &b, &a);
		if (r == 255 && g == 0 && b == 0) dest.y -= speed;

		pixel = pixels[(y + dest.h / 2) * width + (x + dest.w)];  // �������� ���� ������� �������
		SDL_GetRGBA(pixel, SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), &r, &g, &b, &a);
		if (r == 255 && g == 0 && b == 0) dest.x -= speed;
	}

	//������������ ���������
	void ObjectMovement2(const Uint8* keystates) {

		int mas[4];
		for (int i = 0; i < 4; i++)
			mas[i] = 0;

		if (keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN])  mas[0] = 1;
		if (keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP])    mas[1] = 2;
		if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT])  mas[2] = 3;
		if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) mas[3] = 4;

		//���������, ������� �� ������ �� ����� ������ �����
		bool flag0 = true;
		for (int k = 0; k < 4; k++) {
			if (mas[k] != 0) flag0 = false;
		}
		for (int i = 0; i < 4; i++) {
			//� ����� ������� ��������� ����� ���������� �������� �� ����
			if (mas[i] != old_mas[i] && mas[i] != 0) {
				n = i + 1;
				break;
			}
			//� ����� ������� ��������� ����� ���������� ������ ����, ������ ���� ������ �� ������� �� ������ �����
			else if (mas[i] != old_mas[i] && mas[i] == 0 && !flag0) {
				for (int k = 0; k < 4; k++)
					if (mas[k] != 0) n = k + 1;
			}
		}
		if (flag0) n = 0;

		if (n == 2)       dest.y -= speed;
		else if (n == 3)  dest.x -= speed;
		else if (n == 1)  dest.y += speed;
		else if (n == 4)  dest.x += speed;


		for (int i = 0; i < 4; i++) {
			old_mas[i] = mas[i];
		}
	}

	//�������� ���� �������
	void WlakMap(SDL_Rect& dest2) {

		int window_width, window_height;
		SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

		if (dest.x + dest.w > (window_width / 2 + 150)) {
			dest.x = (window_width / 2 + 150) - dest.w;
			dest2.x -= speed;
		}
		if (dest.x < (window_width / 2 - 150)) {
			dest.x = (window_width / 2 - 150);
			dest2.x += speed;
		}
		if (dest.y > (window_height / 2 + 150) - dest.h) {
			dest.y = (window_height / 2 + 150) - dest.h;
			dest2.y -= speed;
		}
		if (dest.y < (window_height / 2 - 150)) {
			dest.y = (window_height / 2 - 150);
			dest2.y += speed;
		}
	}
};

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "RUS");

	SDL_Window* window = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1250, 650, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//�������� ����� (��������)
	Character pers = Character(10, 400);

	//������� �������, ������������� ����
	ObjectWithShadow2 obj_1 = ObjectWithShadow2(330, 20, 340, 20, 340, 130, 330, 130);
	ObjectWithShadow2 obj_2 = ObjectWithShadow2(695, 20, 710, 20, 710, 160, 695, 160);
	ObjectWithShadow2 obj_3 = ObjectWithShadow2(695, 160, 710, 160, 710, 335, 695, 335);
	ObjectWithShadow2 obj_4 = ObjectWithShadow2(710, 240, 790, 240, 790, 250, 710, 250);
	ObjectWithShadow2 obj_5 = ObjectWithShadow2(860, 240, 940, 240, 940, 250, 860, 250);
	ObjectWithShadow2 obj_6 = ObjectWithShadow2(330, 320, 340, 320, 340, 345, 330, 345);
	ObjectWithShadow2 obj_7 = ObjectWithShadow2(340, 335, 525, 335, 525, 345, 340, 345);
	ObjectWithShadow2 obj_8 = ObjectWithShadow2(525, 335, 710, 335, 710, 345, 525, 345);

	//������� �������, �� ������� ������������� ����
	Wall wall_1 = Wall(20, 80, 330, 80, 60);
	Wall wall_2 = Wall(340, 80, 695, 80, 60);
	Wall wall_3 = Wall(710, 80, 940, 80, 60);
	Wall wall_4 = Wall(710, 250, 790, 250, 80);
	Wall wall_5 = Wall(860, 250, 940, 250, 80);
	Wall wall_6 = Wall(330, 345, 710, 345, 75);

	// ������� �����
	SDL_Texture* Borders = LoadImage("borders_5.png");
	SDL_Rect Borders_dest;
	SDL_QueryTexture(Borders, NULL, NULL, &Borders_dest.w, &Borders_dest.h);
	Borders_dest.x = 0;
	Borders_dest.y = 0;

	// ������ ���
	SDL_Texture* BackGround = LoadImage("back_gr_5.png");
	SDL_Rect background_dest;
	SDL_QueryTexture(BackGround, NULL, NULL, &background_dest.w, &background_dest.h);
	background_dest.x = 0;
	background_dest.y = 0;

	// ����� ����
	SDL_Texture* shadow = LoadImage("shadow_pr.png");
	SDL_Rect dest_shadow;
	SDL_QueryTexture(shadow, NULL, NULL, &dest_shadow.w, &dest_shadow.h);
	dest_shadow.w /= 1;
	dest_shadow.h /= 1;
	dest_shadow.x = pers.dest.x - 750;
	dest_shadow.y = pers.dest.x - 750;

	const Uint8* keystates = SDL_GetKeyboardState(NULL);

	int close = 0;
	while (!close) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {// ���������� ���������
			switch (event.type) {
			case SDL_QUIT:
				// ���������� ������� ��������
				close = 1;
				break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		//������ � ���������, �� ������� ������������� ����
		ObjectWithShadow2* objects = new ObjectWithShadow2[obj_count]{ obj_1, obj_2, obj_3, obj_4, obj_5, obj_6, obj_7, obj_8 };

		//������ � ���������, �� ������� ������������� ����
		Wall* walls = new Wall[wall_count]{ wall_1, wall_2, wall_3, wall_4, wall_5, wall_6 };

		//������� ������ ����� ����� ��������� ������������� � ����� �������
		pers.dest.x -= pers.dest.w / 2;
		pers.dest.y -= pers.dest.h / 2;

		SDL_RenderCopy(renderer, Borders, NULL, &Borders_dest);
		pers.ObjectMovement2(keystates);  //������������ ���������

		pers.WlakMap(background_dest); //������������ �������
		Borders_dest = background_dest; //������������ ������ �������

		pers.InteractionWithBorders(renderer);  //��������� �� ������� �������

		SDL_RenderCopy(renderer, BackGround, NULL, &background_dest);
		pers.GetCorrectAnim();  //��������� ���������� ��� ���������� ��������

		pers.CreateAnimations();  //������ ���������
		pers.dest.x += pers.dest.w / 2;
		pers.dest.y += pers.dest.h / 2;

		//��������� ��������, ������� ������ ���� ������ ���������
		//...

		//����������� �������� � ���� ������������ ������� ����
		for (int i = 0; i < obj_count; i++) 
			objects[i].move_obj(background_dest);
		for (int i = 0; i < wall_count; i++)
			walls[i].move_wall(background_dest);

		//��������� �������� ���� �� ��� ����
		ApplyingGeneralShadow(objects, pers.dest, dest_shadow, shadow, walls);

		//��������� ������ �������� � ���� ��� ��������� ����� �� �������
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		for (int i = 0; i < obj_count; i++) 
			objects[i].drawObject();
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		for (int i = 0; i < wall_count; i++) 
			walls[i].DrawWall();

		SDL_RenderPresent(renderer);

		SDL_Delay(1000 / 60);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}