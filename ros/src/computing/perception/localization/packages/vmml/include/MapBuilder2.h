/*
 * MapBuilder2.h
 *
 *  Created on: Jul 26, 2018
 *      Author: sujiwo
 */

#ifndef MAPBUILDER2_H_
#define MAPBUILDER2_H_

#include <vector>
#include <string>
#include <Eigen/Eigen>
#include <Eigen/Geometry>
#include <opencv2/core.hpp>
#include <functional>
#include "VMap.h"
#include "utilities.h"

#include "datasets/GenericDataset.h"


class Viewer;

const double
	translationThrs = 1.0,	// meter
	rotationThrs = 0.0175;	// == 1 degrees



struct InputFrame
{
	cv::Mat image;
	Eigen::Vector3d position = Eigen::Vector3d::Zero();
	Eigen::Quaterniond orientation = Eigen::Quaterniond::Identity();
	bool positionIsValid = true;
	uint cameraId = 0;
	dataItemId sourceId=std::numeric_limits<dataItemId>::max();
	ptime tm=boost::posix_time::not_a_date_time;

	InputFrame() {}

	InputFrame (
			const cv::Mat &i,
			const Eigen::Vector3d &p,
			const Eigen::Quaterniond &o,
			dataItemId _forceSrcId=std::numeric_limits<dataItemId>::max()) :

		image(i),
		position(p),
		orientation(o),
		sourceId(_forceSrcId)
	{}

	Pose getPose() const
	{ return Pose::from_Pos_Quat(position, orientation); }
};


class MapBuilder2
{

public:

	MapBuilder2 ();
	virtual ~MapBuilder2();

	void input (const InputFrame &f);

	void build ();

	VMap* getMap()
	{ return cMap; }

	void addCameraParam (const CameraPinholeParams &c);

	inline kfid getCurrentKeyFrameId()
	{ return kfAnchor; }

	void resetMap();

	typedef std::function<void(const InputFrame&)> frameCallback;
	inline void registerFrameCallback (frameCallback& f)
	{ inputCallback = f; }

	void runFromDataset
		(GenericDataset::Ptr sourceDs,
		const ptime startTime,
		const ptime stopTime);

	void runFromDataset
		(GenericDataset::Ptr sourceDs,
			dataItemId startPos=std::numeric_limits<dataItemId>::max(),
			dataItemId stopPos=std::numeric_limits<dataItemId>::max()
		);

	void setMask (const cv::Mat &m);

	bool
	checkDataPoints (GenericDataset::ConstPtr sourceDs, const ptime startTime, const ptime stopTime)
	const;

	bool
	checkDataPoints (GenericDataset::ConstPtr sourceDs, const dataItemId startPos, const dataItemId stopPos)
	const;


protected:

	CameraPinholeParams cparams;

	VMap *cMap;

	cv::Mat mask;

	kfid kfAnchor;
	InputFrame ifrAnchor;

	bool initialized = false;

	GenericDataset::Ptr sourceDataset=nullptr;
	frameCallback inputCallback;

	InputFrame frame0;

private:
	static bool isNormalFrame (const InputFrame &f);

	void initialize (const InputFrame &f1, const InputFrame &f2);

	void track (const InputFrame &f);

	void mapPointCulling();

};

#endif /* MAPBUILDER2_H_ */