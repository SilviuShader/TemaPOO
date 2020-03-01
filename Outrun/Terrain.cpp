#include "pch.h"
#include "Terrain.h"

using namespace std;

using namespace DirectX::SimpleMath;

Terrain::Line::Line() :
	m_position(Vector3::Zero)
{
}

Terrain::Line::Line(Vector3 position) :
	m_position(position)
{
}

Terrain::Line::Line(const Line& other) :
	m_position(other.m_position)
{
}

Terrain::Line::~Line()
{
}

Terrain::Terrain(int roadWidth,
	             int segmentLength,
	             int linesCount) :
	
	m_roadWidth(roadWidth),
	m_segmentLength(segmentLength),
	m_linesCount(linesCount),
	m_lines(vector<Line>())

{
	for (int i = 0; i < m_linesCount; i++)
	{
		Line line = Line();
		Vector3 linePos = line.GetPosition();
		linePos.z = i * m_segmentLength;
		line.SetPosition(linePos);

		m_lines.push_back(line);
	}
}

Terrain::Terrain(const Terrain& other)
{
}

Terrain::~Terrain()
{
}
