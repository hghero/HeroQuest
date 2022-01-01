#include "Math.h"

using namespace std;


const double Line::EPSILON_DETERMINANT = 0.00001;


ostream& operator<<(ostream& stream, const Vec2i& vec)
{
	stream << vec.toString().toStdString();
	return stream;
}

ostream& operator<<(ostream& stream, const Vec2f& vec)
{
	stream << vec.toString().toStdString();
	return stream;
}

ostream& operator<<(ostream& stream, const Vec2d& vec)
{
	stream << vec.toString().toStdString();
	return stream;
}

size_t randomNumber(size_t limit)
{
    size_t r = rand() % limit;
    return r;
}

/*!
 * If l1 and l2 intersect, the intersection point is stored in s.
 *
 * \return True, if l1 intersects l2, false otherwise.
 */
bool intersect(const Line& l1, const Line& l2, Vec2d* s)
{
	s->x = DBL_MAX;
	s->y = DBL_MAX;

	// s = l1.p1 + lambda * (l1.p2 - l1.p1) = l2.p1 + mu * (l2.p2 - l2.p1)
    //
	//                             <=>
	//
	//     lambda * (l1.p2 - l1.p1) + mu * (l2.p1 - l2.p2) = l2.p1 - l1.p1
    //
	//                             <=>
	//
	//     lambda * a + mu * b = c             with
	//                                           a = l1.p2 - l1.p1
	//                                           b = l2.p1 - l2.p2
	//                                           c = l2.p1 - l1.p1
    //
	//                             <=>
	//
	//     lambda * a.x + mu * b.x = c.x         
	//     lambda * a.y + mu * b.y = c.y         with
	//                                           a = l1.p2 - l1.p1
	//                                           b = l2.p1 - l2.p2
	//                                           c = l2.p1 - l1.p1

	Vec2d a(l1.getP2() - l1.getP1());
	Vec2d b(l2.getP1() - l2.getP2());
	Vec2d c(l2.getP1() - l1.getP1());

	// 1) are the lines parallel, i.e. is the determinant 0?
	double det_base = a.x * b.y - a.y * b.x;
	if (fabs(det_base) <= Line::EPSILON_DETERMINANT)
		return false;

	// 2) compute mu
	double det_mu = a.x * c.y - a.y * c.x;
	double mu = det_mu / det_base;

	// 3) compute s
	// s = l2.p1 + (l2.p2 - l2.p1) * mu
	*s = Vec2d(l2.getP1()) + Vec2d((l2.getP2() - l2.getP1())) * mu;

	// 4) is s on l1 and on l2?
	if (!l1.contains(*s))
		return false;
	if (!l2.contains(*s))
		return false;

	return true;
}
