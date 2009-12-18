/*! \file Volume.h
	\brief Volume header file.

	Contains the declarations and source code for the Volume and Volume::Voxel classes which are used to represent the volume dataset.
*/

#pragma once

#include <QDebug>
#include "common.h"
#include <math.h>
#include <stdlib.h>
//#include "glew.h>
//#include <openglut.h>
#include <vector>

#include <algorithm>
#include <functional>

//! Simple linear volume class which supports loading from DAT files
class Volume
{
public:

	//! Helper class the represents a single voxel
	class Voxel
	{
	public:

		//! Default constructor.
		/*!
			Creates a voxel with a density value of 0.
			\sa Voxel(Voxel &), Voxel(const float), ~Voxel()
		*/
		Voxel()
		{
			SetValue(0.0f);
		};

		//! Copy constructor.
		/*!
			Creates a copy of an existing voxel.
			\param datOther The voxel to copy.
			\sa Voxel(), Voxel(const float), ~Voxel()
		*/
		Voxel(const Voxel & datOther)
		{
			m_fValue = datOther.m_fValue;
		};

		//! Constructor.
		/*!
			Creates a voxel with a given value.
			\param fValue The value to set the voxel's density to.
			\sa Voxel(), Voxel(const Voxel &) ~Voxel()
		*/
		Voxel(const float fValue)
		{
			SetValue(fValue);
		};


		//! Default destructor.
		/*!
			\sa Voxel(), Voxel(const Voxel &), Voxel(const float), ~Voxel()
		*/
		~Voxel()
		{
		};

		//! Setter for the voxel's density value.
		/*!
			\param fValue The value to set the voxel's density to.
			\sa GetValue()
		*/
		void SetValue(const float fValue)
		{
			m_fValue = fValue;
		};

		//! Getter for the voxel's density value.
		/*!
			\return The voxel's density value.
			\sa SetValue()
		*/
		const float GetValue() const
		{
			return m_fValue;
		};

		//! Equality operator.
		/*!
			\param datOther The other voxel to compare this one to.
			\return \a true if this voxel's density value is equal to the other voxel's, \a false if it isn't.
			\sa operator!=(), operator>(), operator>=(), operator<(), operator<=()
		*/
		const bool operator==(const Voxel &datOther) const
		{
			return (GetValue() == datOther.GetValue());
		};

		//! Inequality operator.
		/*!
			\param datOther The other voxel to compare this one to.
			\return \a true if this voxel's density value is not equal to the other voxel's, \a false if it is.
			\sa operator==(), operator>(), operator>=(), operator<(), operator<=()
		*/
		const bool operator!=(const Voxel &datOther) const
		{
			return !(*this == datOther);
		};

		//! Greater-than operator.
		/*!
			\param datOther The other voxel to compare this one to.
			\return \a true if this voxel's density value is greater than the other voxel's, \a false if it isn't.
			\sa operator==(), operator!=(), operator>=(), operator<(), operator<=()
		*/
		const bool operator>(const Voxel &datOther) const
		{
			return GetValue() > datOther.GetValue();
		};

		//! Greater-than-or-equal operator.
		/*!
			\param datOther The other voxel to compare this one to.
			\return \a true if this voxel's density value is greater than or equal to the other voxel's, \a false if it isn't.
			\sa operator==(), operator!=(), operator>(), operator<(), operator<=()
		*/
		const bool operator>=(const Voxel &datOther) const
		{
			return GetValue() >= datOther.GetValue();
		};

		//! Less-than operator.
		/*!
			\param datOther The other voxel to compare this one to.
			\return \a true if this voxel's density value is less than the other voxel's, \a false if it isn't.
			\sa operator==(), operator!=(), operator>(), operator>=(), operator<=()
		*/
		const bool operator<(const Voxel &datOther) const
		{
			return GetValue() < datOther.GetValue();
		};

		//! Less-than-or-equal operator.
		/*!
			\param datOther The other voxel to compare this one to.
			\return \a true if this voxel's density value is less than or equal to the other voxel's, \a false if it isn't.
			\sa operator==(), operator!=(), operator>(), operator>=(), operator<()
		*/
		const bool operator<=(const Voxel &datOther) const
		{
			return GetValue() <= datOther.GetValue();
		};

		//! Assignment and addition operator.
		/*!
			Adds another voxel's density value to this one's.
			\param datOther The other voxel to add to this one.
			\return This voxel.
			\sa operator-=(), operator*=(), operator/=(), operator+(), operator-(), operator*(), operator/()
		*/
		const Voxel & operator+=(const Voxel & datOther)
		{
			m_fValue += datOther.m_fValue;
			return *this;
		};

		//! Assignment and subtraction operator.
		/*!
			Subtracts another voxel's density value from this one's.
			\param datOther The other voxel to subtract from this one.
			\return This voxel.
			\sa operator+=(), operator*=(), operator/=(), operator+(), operator-(), operator*(), operator/()
		*/
		const Voxel & operator-=(const Voxel & datOther)
		{
			m_fValue -= datOther.m_fValue;
			return *this;
		};

		//! Assignment and multiplication operator.
		/*!
			Multiplies this voxel's density value by a certain factor.
			\param fOther The factor to multiply this voxel's density value by.
			\return This voxel.
			\sa operator+=(), operator-=(), operator/=(), operator+(), operator-(), operator*(), operator/()
		*/
		const Voxel & operator*=(const float & fOther)
		{
			m_fValue *= fOther;
			return *this;
		};

		//! Assignment and division operator.
		/*!
			Divides this voxel's density value by a certain factor.
			\param fOther The factor to divide this voxel's density value by.
			\return This voxel.
			\sa operator+=(), operator-=(), operator*=(), operator+(), operator-(), operator*(), operator/()
		*/
		const Voxel & operator/=(const float & fOther)
		{
			m_fValue /= fOther;
			return *this;
		};

		//! Addition operator.
		/*!
			Adds another voxel's density value to this one's.
			\param datOther The other voxel to add to this one.
			\return A voxel with the sum of the density values of this one and the other's.
			\sa operator+=(), operator-=(), operator*=(), operator/=(), operator-(), operator*(), operator/()
		*/
		const Voxel operator+(const Voxel & datOther) const
		{
			Voxel voxNew = *this;
			voxNew += datOther;
			return voxNew;
		};

		//! Subtraction operator.
		/*!
			Subtracts another voxel's density value from this one's.
			\param datOther The other voxel to subtract from this one.
			\return A voxel with the difference of the density values of this one and the other's.
			\sa operator+=(), operator-=(), operator*=(), operator/=(), operator+(), operator*(), operator/()
		*/
		const Voxel operator-(const Voxel & datOther) const
		{
			Voxel voxNew = *this;
			voxNew -= datOther;
			return voxNew;
		};

		//! Multiplication operator.
		/*!
			Multiplies this voxel's density value by a certain factor.
			\param fOther The factor to multiply this voxel's density value by.
			\return A voxel with a density value equal to this voxel's multiplied by the given factor.
			\sa operator+=(), operator-=(), operator*=(), operator/=(), operator+(), operator-(), operator/()
		*/
		const Voxel operator*(const float & fOther) const
		{
			Voxel voxNew = *this;
			voxNew *= fOther;
			return voxNew;
		};

		//! Division operator.
		/*!
			Divides this voxel's density value by a certain factor.
			\param fOther The factor to divide this voxel's density value by.
			\return A voxel with a density value equal to this voxel's divided by the given factor.
			\sa operator+=(), operator-=(), operator*=(), operator/=(), operator+(), operator-(), operator*()
		*/
		const Voxel operator/(const float & fOther) const
		{
			Voxel voxNew = *this;
			voxNew /= fOther;
			return voxNew;
		};

	private:

		//! This node's density value.
		float m_fValue;
	};

public:
	
	//! Default constructor.
	/*!
		Constructs a 1x1x1 volume with a single voxel of 0 density.
		\sa Volume(const std::string &), ~Volume()
	*/
	Volume() : m_iWidth(1), m_iHeight(1), m_iDepth(1), m_vecVoxels(1)
	{
	};

	//! Constructor.
	/*!
		Constructs a volume from a data file.
		\param strFilename The file name of the volume data file to load.
		\sa load(), Volume(), ~Volume()
	*/
	Volume(const std::string &strFilename) : m_iWidth(1), m_iHeight(1), m_iDepth(1), m_vecVoxels(1)
	{
		load(strFilename);
	};

	//! Default destructor.
	/*!
		\sa Volume()
	*/
	~Volume(void)
	{
	};

	//! Getter for a single voxel by coordinate.
	/*!
		\param iX The voxel's x-coordinate.
		\param iY The voxel's y-coordinate.
		\param iZ The voxel's z-coordinate.
		\return The voxel at the given coordinates.
		\sa Get()
	*/
	const Voxel & Get(const int iX, const int iY, const int iZ) const
	{
		return m_vecVoxels[iX + iY*m_iWidth + iZ*m_iWidth*m_iHeight];
	};

	//! Getter for a single voxel by index.
	/*!
		\param iIndex The voxel's index.
		\return The voxel with the given index.
		\sa Get()
	*/
	const Voxel & Get(const int iIndex) const
	{
		return m_vecVoxels[iIndex];
	};

	//! Getter for a pointer to the first voxel.
	/*!
		Wrapper function for the underlying vector's front().
		\return A pointer pointing at the first voxel.
		\sa Get()
	*/
	const Voxel * Get() const
	{
		return &(m_vecVoxels.front());
	};

	//! Getter for the volume's width.
	/*!
		\return The volume's width.
		\sa GetHeight(), GetDepth()
	*/
	const int GetWidth() const
	{
		return m_iWidth;
	};

	//! Getter for the volume's height.
	/*!
		\return The volume's height.
		\sa GetWidth(), GetDepth(), GetSize()
	*/
	const int GetHeight() const
	{
		return m_iHeight;
	};

	//! Getter for the volume's depth.
	/*!
		\return The volume's depth.
		\sa GetHeight(), GetDepth(), GetSize()
	*/
	const int GetDepth() const
	{
		return m_iDepth;
	};

	//! Getter for the volume's number of voxels.
	/*!
		Wrapper function for the underlying vector's size().
		\return The volume's number of voxels.
		\sa GetWidth(), GetHeight(), GetDepth()
	*/
	const int GetSize() const
	{
		return int(m_vecVoxels.size());
	};
	
	//! Loads a volume data set from a DAT file
	/*!
		Sets the dimensions of this volume and loads the density data for every voxel in it, converting them to float values in [0..1].
		Also generates the histogram for this volume data set.
		\param strFilename The file name of the data set to load.
		\sa Volume()
	*/
	void load(const std::string & strFilename)
	{
		for (unsigned int i = 0; i < 256; i++) {
			histogram[i] = 0;
		}

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
						unsigned int j = int(floor(fValue * 255));
						//if (j > 0)
						//{
							//histogram[int(floor(fValue * 255))] += 1;
							//qDebug() << int(floor(fValue * 255));
							histogram[j] += 1;
						//}
					}
				}
				//qDebug() << "Preparing Data";
				//qDebug() << k*100;
				std::cout << "\r- Preparing data (" << (k*100) / (m_iDepth-1) << "%) ...";

			}
			qDebug() << "Data prepared";
			std::cout << std::endl << "- Data prepared." << std::endl;

			int histHeight = 0;
			//unsigned int t = 0;
			for (int i = 1; i < 256; i++) {
				if (histogram[i] > histHeight)
					histHeight = histogram[i];
					//t = i;
			}
			//qDebug() << "t: " << t;
			//qDebug() << "h: " << histHeight;
		}

	};

	//! Getter for a pointer to access this volume's histogram.
	/*!
		\return A pointer to this volume's histogram.
	*/
	int* getHistogram(void) {
		return histogram;
	};

protected:

private:
	
	//! The vector containing this volume's voxels.
	std::vector<Voxel> m_vecVoxels;

	//! This volume's width in voxels.
	int m_iWidth;

	//! This volume's height in voxels.
	int m_iHeight;

	//! This volume's depth in voxels.
	int m_iDepth;

	//! This volume's histogram.
	int histogram[256];
};
