/*
Copyright © 2016 Jonathan Shirlaw

File: Bezier.h
Author: Jonathan Shirlaw
Date: 02/06/2016

Purpose: Class to handle an N-point Bezier curve
*/
#pragma once
#include <vector>

namespace Math
{
	template <typename VecType>
	class Bezier
	{
	private:
		std::vector<VecType> _pointList;

	public:

		// Adds a point to the Bezier
		void AddPoint(const VecType& refPoint)
		{
			_pointList.push_back(refPoint);
		}

		// Get the point at index
		VecType GetPoint(int nIndex) const
		{
			// Bounds check
			if (nIndex < 0 || nIndex >= (int)_pointList.size())
				return VecType();

			return _pointList[nIndex];
		}

		// Get the number of points in the Bezier
		int PointCount() const
		{
			return _pointList.size();
		}

		// Moves a point at index to a new position
		void MovePoint(int nIndex, const VecType& refPoint)
		{
			// Bounds check
			if (nIndex < 0 || nIndex >= (int)_pointList.size())
				return;

			// Set the point to its new position
			_pointList[nIndex] = refPoint;
		}

		// Returns a point along the curve, takes in 0.0f - 1.0f
		VecType Delta(float fDelta)
		{
			// Make sure we have at least 2 points
			if (_pointList.size() < 2)
				return VecType();

			// Start the recursion
			return Delta(_pointList, fDelta);
		}

		// Returns a list of the Beziers points
		const std::vector<VecType>& GetPoints() const
		{
			return _pointList;
		}

	protected:
		// Internal helper method to calculate the points recursively
		VecType Delta(const std::vector<VecType>& refPoints, float fDelta)
		{
			// Base case
			if (refPoints.size() == 2)
				// Return the point along the curve at the given delta
				return Lerp(refPoints[0], refPoints[1], fDelta);


			std::vector<VecType> subPoints;

			// We start at i = 1, so we can Lerp from i-1 to i, this will reduce our number of points by 1 for each recursion
			for (int i = 1; i < (int)refPoints.size(); i++)
				subPoints.push_back(Lerp(refPoints[i - 1], refPoints[i], fDelta));
			
			// Now we have lerped between all the current points, we recurse using the new list
			return Delta(subPoints, fDelta);
		}

		// TODO: Move this Lerp to a better place
		VecType Lerp(const VecType& refPoint1, const VecType& refPoint2, float fDelta)
		{
			return refPoint1 + fDelta * (refPoint2 - refPoint1);
		}
	};
}