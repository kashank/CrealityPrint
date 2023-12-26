#include "spreadmodel.h"
#include "msbase/space/intersect.h"
#include "spreadchunk.h"
#include "qtuser3d/refactor/xeffect.h"
#include <QtMath>

#define MAX_COLOR_NUM 16

SpreadModel::SpreadModel(QObject* object)
	: QObject(object)
{
	m_effect = new qtuser_3d::XEffect();
	{
		qtuser_3d::XRenderPass* viewPass = new qtuser_3d::XRenderPass("spread", m_effect);
		viewPass->addFilterKeyMask("view", 0);
		viewPass->addFilterKeyMask("rt", 0);
		viewPass->setPassCullFace();
		viewPass->setPassDepthTest();
		m_effect->addRenderPass(viewPass);

		qtuser_3d::XRenderPass* pickPass = new qtuser_3d::XRenderPass("pickFace_pwd", m_effect);
		pickPass->addFilterKeyMask("pick", 0);
		pickPass->setPassBlend(Qt3DRender::QBlendEquationArguments::One, Qt3DRender::QBlendEquationArguments::Zero);
		pickPass->setPassCullFace();
		pickPass->setPassDepthTest();
		m_effect->addRenderPass(pickPass);
	}

}

SpreadModel::~SpreadModel()
{
	clear();
	m_effect->deleteLater();
}

void SpreadModel::init()
{
	setSection(QVector3D(0.0, 0.0, 100000.0), QVector3D(0.0, 0.0, -10000.0), QVector3D(0.0, 0.0, -1.0));
	m_effect->setParameter("internalColor", QVector4D(0.88, 0.55, 0.18, 1.0));
}

void SpreadModel::clear()
{
	m_effect->setParent((Qt3DCore::QNode*)NULL);
	qDeleteAll(m_chunks);
	m_chunks.clear();
}

void SpreadModel::addChunk(const std::vector<trimesh::vec3>& positions, const std::vector<int>& flags)
{
	SpreadChunk* chunk = new SpreadChunk(m_chunks.size(), this);
	chunk->updateData(positions, flags);
	chunk->setEffect(m_effect);
	m_chunks << chunk;
}

int SpreadModel::chunkCount()
{
	return m_chunks.size(); 
}

void SpreadModel::updateChunkData(int id, const std::vector<trimesh::vec3>& position, const std::vector<int>& flags)
{
	if ((chunkCount() <= id) || id < 0)
		return;

	SpreadChunk* chunk = m_chunks[id];
	chunk->updateData(position, flags);
}

void SpreadModel::setChunkIndexMap(int id, const std::vector<int>& indexMap)
{
	if ((chunkCount() <= id) || id < 0)
		return;

	SpreadChunk* chunk = m_chunks[id];
	chunk->setIndexMap(indexMap);
}

void SpreadModel::setPose(const QMatrix4x4& pose)
{
	for (int i = 0, count = m_chunks.size(); i < count; ++i)
	{
		m_chunks[i]->setPose(pose);
		/* test */
		// QMatrix4x4 p = pose;
		// p.translate(30 * i, 0);
		// m_chunks[i]->setPose(p);
	}
}

QMatrix4x4 SpreadModel::pose() const
{
	QMatrix4x4 p;
	if (m_chunks.size() > 0)
		p = m_chunks.first()->pose();
	return p;
}

void SpreadModel::setSection(const QVector3D &frontPos, const QVector3D &backPos, const QVector3D &normal)
{
	m_effect->setParameter("sectionNormal", normal);
	m_effect->setParameter("sectionFrontPos", frontPos);
	m_effect->setParameter("sectionBackPos", backPos);
}

void SpreadModel::setRenderModel(int model)
{
	m_effect->setParameter("renderModel", model);
}

void SpreadModel::setPalette(const std::vector<trimesh::vec>& colorsList)
{
	if (colorsList.size() <= 0)
		return;

	QVariantList palette;
	for (int i = 0, colorsCount = colorsList.size(); i < MAX_COLOR_NUM; ++i)
	{
		int _i = i < colorsCount ? i : 0;	
		auto color = colorsList[_i];
		QVector4D qcolor = QVector4D(color[0], color[1], color[2], 1.0);
		palette << qcolor;
	}

	m_effect->setParameter("palette[0]", palette);
}

int SpreadModel::chunkId(qtuser_3d::Pickable* pickable)
{
	for (int i = 0; i < m_chunks.count(); ++i)
	{
		if (m_chunks[i] == pickable)
			return i;
	}
	return -1;
}

bool SpreadModel::getChunkFace(int chunkId, int faceId, trimesh::vec3& p1, trimesh::vec3& p2, trimesh::vec3& p3)
{
	if (chunkId >= m_chunks.count() || chunkId < 0)
		return false;

	return m_chunks[chunkId]->getFace(faceId, p1, p2, p3);
}

int SpreadModel::spreadFaceParentId(int chunkId, int faceId)
{
	if (chunkId >= m_chunks.count() || chunkId < 0)
		return false;

	return m_chunks[chunkId]->parentId(faceId);
}

void SpreadModel::setHighlightOverhangsDeg(float deg)
{
	m_effect->setParameter("highlightOverhangsRadian", qDegreesToRadians(deg));
}