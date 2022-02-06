#pragma once

/**
* Stores any kind of data which has been calculated based on the raw stats
*/
class CalculatedData
{
public:
	CalculatedData() = default;

	double SuccessPercentage = .0;		///< The total percentage of goals scored in relation to attempts made
	double PeakSuccessPercentage = .0;	///< The peak percentage of goals scored vs attempts, based on the last N shots (e.g. 50)
	int PeakShotNumber = 0;				///< The shot number at which the peak occurred
	double Last50ShotsPercentage = .0;	///< The percentage of the last 50 shots
	double InitialHitPercentage = .0;	///< The percentage of attempts where the ball was hit at least one. (Useful for speedflip training).

	/** Compares this object to other and returns the result as a new CalculatedData instance. 
	 *  The resulting percentages will be positive if "this" is better than "other".
	 * 
	 * We only compare values which make sense to be compared, i.e. we don't compare stats which reference only a part of the shots made.
	 */
	CalculatedData getDifferences(const CalculatedData& other) const 
	{
		CalculatedData diff;
		diff.SuccessPercentage = SuccessPercentage - other.SuccessPercentage;
		diff.PeakSuccessPercentage = PeakSuccessPercentage - other.PeakSuccessPercentage;
		diff.InitialHitPercentage = InitialHitPercentage - other.InitialHitPercentage;
		return diff;
	}
};