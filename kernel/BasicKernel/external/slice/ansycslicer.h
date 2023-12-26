#ifndef _NULLSPACE_ANSYCSLICER_1591673316708_H
#define _NULLSPACE_ANSYCSLICER_1591673316708_H
#include "crslice/gcode/sliceresult.h"
#include "us/usettings.h"
#include "qtusercore/module/progressor.h"
#include "qtusercore/module/progressortracer.h"
#include "crslice/header.h"

namespace creative_kernel
{
	class ModelGroupInput;
	class SliceInput : public QObject
	{
	public:
		SliceInput(QObject* parent = nullptr);
		~SliceInput();

		bool canSlice();
		trimesh::box3 sceneBox();
		bool hasModel();
		QList<ModelGroupInput*> Groups;
		SettingsPointer G;
		QList<SettingsPointer> Es;
	};

	class AnsycSlicer : public QObject
	{
	public:
		AnsycSlicer(QObject* parent = nullptr);
		virtual ~AnsycSlicer();

		virtual SliceResultPointer doSlice(SliceInput& input, qtuser_core::ProgressorTracer& tracer, crslice::PathData* _fDebugger = nullptr);

		virtual void stop();
	};

	void produceSliceInput(SliceInput& input);
}
#endif // _NULLSPACE_ANSYCSLICER_1591673316708_H
