#ifndef PAPLACE_H_
#define PAPLACE_H_

#include <vector>
#include <Eigen/Dense>
#include <iostream>


#include "db/Database.h"
#include <pybind11/embed.h>
namespace py = pybind11;

PROJECT_NAMESPACE_BEGIN

class IdeaPlaceEx;

class PAPlaceData
{
    public:
        explicit PAPlaceData() = default;

        int _numCells;
        Eigen::Matrix<double, Eigen::Dynamic, 1> _vector;
        std::vector<std::string> _names;

        void setNumCells(int numCells)
        {
            _numCells = numCells;
        }

        int getNumCells()
        {
            return _numCells;
        }

        void setNames(std::vector<std::string> &names)
        {
            _names = names;
        }

        std::string getName(int idx)
        {
            return _names[idx];
        }

        void setVector(Eigen::Matrix<double , Eigen::Dynamic, 1> &vector)
        {
            this->_vector = vector;
        }

        double getValue(int idx, int orient)
        {
            return _vector(plIdx(idx, orient));
        }

        double setValue(int idx, int orient, double value)
        {
            _vector(plIdx(idx, orient)) = value;
        }

        inline int plIdx(int cellIdx, int orient)
        {
            if (orient == 0)
            {
                return cellIdx;
            }
            else if (orient ==1)
            {
                return cellIdx + _numCells;
            }
            else
            {
            #ifdef MULTI_SYM_GROUP
                    return cellIdx + 2 *  _numCells; // here cell index representing the idx of sym grp
            #else
                    return 2 * _numCells;
            #endif
            }
        }

        int size()
        {
            return _vector.size();
        }

        void resize(int size)
        {
            _vector.resize(size);

        }
};


class PAPlace
{
    public:
        explicit PAPlace() = default;

        IdeaPlaceEx* placer;
        Database _db; ///< The placement engine database

        inline IndexType plIdx(IndexType cellIdx, Orient2DType orient);

        void setIdeaPlaceEX(IdeaPlaceEx &placer);
        IdeaPlaceEx getIdeaPlaceEX();

        Cell getCellFromIdeaPlaceEX(IndexType cellIdx);

        Eigen::Matrix<double , Eigen::Dynamic, 1> _getGuideGradient();

        Eigen::Matrix<double , Eigen::Dynamic, 1> getGuideGradient()
        {
            return this->_getGuideGradient();
        }
};

PROJECT_NAMESPACE_END

#endif ///PAPLACE_H_