#pragma once

class Terrain
{
public:

	struct Line
	{
	public:

		Line();
		Line(DirectX::SimpleMath::Vector3);
		Line(const Line&);
		~Line();

		inline void                         SetPosition(DirectX::SimpleMath::Vector3 position)  { m_position = position;  };
		inline DirectX::SimpleMath::Vector3 GetPosition()                                 const { return m_position; };

	protected:

		DirectX::SimpleMath::Vector3 m_position;
	};

public:

	Terrain(int, int, int);
	Terrain(const Terrain&);
	~Terrain();

	inline std::vector<Line>& GetLines()               { return m_lines;         }
	inline int                GetRoadWidth()     const { return m_roadWidth;     }
	inline int                GetSegmentLength() const { return m_segmentLength; }

private:

	int               m_roadWidth;
	int               m_segmentLength;
	int               m_linesCount;

	std::vector<Line> m_lines;
};