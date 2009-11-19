#pragma once

#include <QDebug>
#include "common.h"
#include <stdlib.h>
//#include "glew.h>
//#include <openglut.h>
#include <vector>

#include <algorithm>
#include <functional>

#include "Matrix.h"

// Simple linear volume class which supports loading from DAT files
class Volume
{
public:

	class Voxel
	{
	public:

		Voxel()
		{
			SetValue(0.0f);
		};

		Voxel(const Voxel & datOther)
		{
			m_fValue = datOther.m_fValue;
		};

		Voxel(const float fValue)
		{
			SetValue(fValue);
		};


		~Voxel()
		{
		};

		void SetValue(const float fValue)
		{
			m_fValue = fValue;
		};

		const float GetValue() const
		{
			return m_fValue;
		};

		const bool operator==(const Voxel &datOther) const
		{
			return (GetValue() == datOther.GetValue());
		};

		const bool operator!=(const Voxel &datOther) const
		{
			return !(*this == datOther);
		};

		const bool operator>(const Voxel &datOther) const
		{
			return GetValue() > datOther.GetValue();
		};

		const bool operator>=(const Voxel &datOther) const
		{
			return GetValue() >= datOther.GetValue();
		};

		const bool operator<(const Voxel &datOther) const
		{
			return GetValue() < datOther.GetValue();
		};

		const bool operator<=(const Voxel &datOther) const
		{
			return GetValue() <= datOther.GetValue();
		};

		const Voxel & operator+=(const Voxel & datOther)
		{
			m_fValue += datOther.m_fValue;
			return *this;
		};

		const Voxel & operator-=(const Voxel & datOther)
		{
			m_fValue -= datOther.m_fValue;
			return *this;
		};

		const Voxel & operator*=(const float & fOther)
		{
			m_fValue *= fOther;
			return *this;
		};

		const Voxel & operator/=(const float & fOther)
		{
			m_fValue /= fOther;
			return *this;
		};

		const Voxel operator+(const Voxel & datOther) const
		{
			Voxel voxNew = *this;
			voxNew += datOther;
			return voxNew;
		};

		const Voxel operator-(const Voxel & datOther) const
		{
			Voxel voxNew = *this;
			voxNew -= datOther;
			return voxNew;
		};

		const Voxel operator*(const float & fOther) const
		{
			Voxel voxNew = *this;
			voxNew *= fOther;
			return voxNew;
		};

		const Voxel operator/(const float & fOther) const
		{
			Voxel voxNew = *this;
			voxNew /= fOther;
			return voxNew;
		};

	private:
		float m_fValue;
	};

public:
	Volume() : m_iWidth(1), m_iHeight(1), m_iDepth(1), m_vecVoxels(1)
	{
	};

	Volume(const std::string &strFilename) : m_iWidth(1), m_iHeight(1), m_iDepth(1), m_vecVoxels(1)
	{
		load(strFilename);
	};

	~Volume(void)
	{
	};

	const Voxel & Get(const int iX, const int iY, const int iZ) const
	{
		return m_vecVoxels[iX + iY*m_iWidth + iZ*m_iWidth*m_iHeight];
	};

	const Voxel & Get(const int iIndex) const
	{
		return m_vecVoxels[iIndex];
	};

	const Voxel * Get() const
	{
		return &(m_vecVoxels.front());
	};

	const int GetWidth() const
	{
		return m_iWidth;
	};

	const int GetHeight() const
	{
		return m_iHeight;
	};

	const int GetDepth() const
	{
		return m_iDepth;
	};

	const int GetSize() const
	{
		return int(m_vecVoxels.size());
	};
	
	
	void load(const std::string & strFilename)
	{
		std::cout << "- Loading file '" << strFilename << "' ... " << std::endl;
		FILE *fp = NULL;
		
		fopen_s(&fp,strFilename.c_str(),"rb");

		if (!fp)
		{
			std::cerr << "+ Error loading file." << std::endl << std::endl;
		}
		else
		{

			char vcPath[1024];
			char *pFileName = NULL;
                        GetFullPathNameA(strFilename.c_str(),1024,vcPath,&pFileName);
			// char vcDrive[1024], vcDirectory[1024], vcFilename[1024], vcExtension[1024];
			//_splitpath_s(vcPath,vcDrive,vcDirectory,vcFilename,vcExtension);
			//const std::string strAdditionalFilename =  std::string(vcDrive)+std::string(vcDirectory)+std::string(vcFilename)+std::string(".ini");

			//char vpSpacingX[1024],vpSpacingY[1024],vpSpacingZ[1024];
			//GetPrivateProfileString("DatFile","oldDat Spacing X","1.0",vpSpacingX,256,strAdditionalFilename.c_str());
			//GetPrivateProfileString("DatFile","oldDat Spacing Y","1.0",vpSpacingY,256,strAdditionalFilename.c_str());
			//GetPrivateProfileString("DatFile","oldDat Spacing Z","1.0",vpSpacingZ,256,strAdditionalFilename.c_str());

			unsigned short uWidth, uHeight, uDepth;
			fread(&uWidth,sizeof(unsigned short),1,fp);
			fread(&uHeight,sizeof(unsigned short),1,fp);
			fread(&uDepth,sizeof(unsigned short),1,fp);

			m_iWidth = int(uWidth);
			m_iHeight = int(uHeight);
			m_iDepth = int(uDepth);

			std::cout << "width: " << m_iWidth << std::endl;
			std::cout << "height: " << m_iHeight << std::endl;
			std::cout << "depth: " << m_iDepth << std::endl;

			qDebug() << "width:" << m_iWidth;
			qDebug() << "height:" << m_iHeight;
			qDebug() << "depth:" << m_iDepth;

			const int iSlice = m_iWidth * m_iHeight;
			const int iSize = iSlice * m_iDepth;
			m_vecVoxels.resize(iSize);

			std::vector<unsigned short> vecData;
			vecData.resize(iSize);

			fread((void*)&(vecData.front()),sizeof(unsigned short),iSize,fp);
			fclose(fp);

			qDebug() << "File loaded";
			std::cout << "- File loaded." << std::endl;

			for (int k=0;k<m_iDepth;k++)
			{
				for (int j=0;j<m_iHeight;j++)
				{
					for (int i=0;i<m_iWidth;i++)
					{
						//we convert the data to float values in an interval of [0..1]
						const float fValue = std::min(1.0f,float(vecData[i + j*m_iWidth + k*iSlice]) / 4095.0f);
						m_vecVoxels[i+j*m_iWidth+k*iSlice] = Voxel(fValue);
					}
				}
				//qDebug() << "Preparing Data";
				//qDebug() << k*100;
				std::cout << "\r- Preparing data (" << (k*100) / (m_iDepth-1) << "%) ...";
			}
			qDebug() << "Data prepared";
			std::cout << std::endl << "- Data prepared." << std::endl;
		}

	};

protected:

private:
	std::vector<Voxel> m_vecVoxels;
	int m_iWidth,m_iHeight,m_iDepth;
};
