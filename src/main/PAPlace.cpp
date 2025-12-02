#include <vector>
#include "main/PAPlace.h"
#include "db/Database.h"
#include "main/IdeaPlaceEx.h"
#include <iostream>
#include <functional>

PROJECT_NAMESPACE_BEGIN

inline IndexType PAPlace::plIdx(IndexType cellIdx, Orient2DType orient)
{
    if (orient == Orient2DType::HORIZONTAL)
    {
        return cellIdx;
    }
    else if (orient == Orient2DType::VERTICAL)
    {
        return cellIdx + this->placer->_db.numCells();
    }
    else
    {
    #ifdef MULTI_SYM_GROUP
            return cellIdx + 2 *  this->placer->_db.numCells(); // here cell index representing the idx of sym grp
    #else
            return 2 * this->placer->_db.numCells();
    #endif
    }
}

Eigen::Matrix<double , Eigen::Dynamic, 1> PAPlace::_getGuideGradient()
{
    Eigen::Matrix<double, Eigen::Dynamic, 1> origin;

    origin.resize(2);
    origin(0) = this->placer->xCellLoc(0);
    origin(1) = this->placer->yCellLoc(0);

    Eigen::Matrix<double , Eigen::Dynamic, 1> results1;
    Eigen::Matrix<double , Eigen::Dynamic, 1> results2;
    Eigen::Matrix<double , Eigen::Dynamic, 1> grad;

    IndexType _numCells = this->placer->_db.numCells();
    IntType size = this->placer->_db.numCells() * 2 + _db.numSymGroups();

    results1.resize(size);
    results2.resize(size);
    grad.resize(size);

    std::vector<std::string> names;

    for (IndexType cellIdx = 0; cellIdx < this->placer->_db.numCells(); cellIdx++)
    {
        Cell cell = getCellFromIdeaPlaceEX(cellIdx);

        LocType x_offset = this->placer->xCellLoc(cellIdx) - origin[0];
        LocType y_offset = this->placer->yCellLoc(cellIdx) - origin[1];

        std::string name = cell.name();

        names.push_back(name);

        results1(plIdx(cellIdx, Orient2DType::HORIZONTAL)) = (x_offset + cell.xLo()) * 0.001;
        results1(plIdx(cellIdx, Orient2DType::VERTICAL))   = (y_offset + cell.yLo()) * 0.001;

        results2(plIdx(cellIdx, Orient2DType::HORIZONTAL)) = (cell.xHi() - cell.xLo()) * 0.001;
        results2(plIdx(cellIdx, Orient2DType::VERTICAL))   = (cell.yHi() - cell.yLo()) * 0.001;
    }

    PAPlaceData r1 = PAPlaceData();
    PAPlaceData r2 = PAPlaceData();

    r1.setNumCells(this->placer->_db.numCells());
    r2.setNumCells(this->placer->_db.numCells());

    r1.setVector(results1);
    r2.setVector(results2);

    r1.setNames(names);
    r2.setNames(names);

    PAPlaceData gradData = PAPlaceData();
    gradData.setNumCells(this->placer->_db.numCells());
    gradData.setVector(grad);

    PAPlaceData *pGradData = &gradData;
    py::object objGradData = py::cast(pGradData);

    auto gradient_manager = py::module_::import("gradient_manager");
    gradient_manager.attr("guidePlacement")(_numCells, r1, r2, objGradData);

    for (IndexType cellIdx = 0; cellIdx < this->placer->_db.numCells(); cellIdx++)
    {
        std::cout << pGradData->getValue(cellIdx, 0) << " "<< pGradData->getValue(cellIdx, 1) << std::endl;
    }

    // auto gradient_manager = py::module_::import("gradient_manager");
    // PAPlaceData *guidedGrad  = gradient_manager.attr("guidePlacement")(_numCells, r1, r2, gradData).cast<PAPlaceData* >();

    // for (int i = 0; i < 100; i++)
    // {
    //     std::cout << "*********************************After Guided*********************************" << std::endl;
    // }

    // for (IndexType cellIdx = 0; cellIdx < this->placer->_db.numCells(); cellIdx++)
    // {
    //     std::cout << guidedGrad->getValue(cellIdx, 0) << " "<< guidedGrad->getValue(cellIdx, 1) << std::endl;
    // }

    return pGradData->_vector;
}


void PAPlace::setIdeaPlaceEX(IdeaPlaceEx &placer)
{
    this->placer = &placer;
    // copy database from IdeaPlace
    this->_db = this->placer->_db;
}


IdeaPlaceEx PAPlace::getIdeaPlaceEX()
{
    return *this->placer;
}

Cell PAPlace::getCellFromIdeaPlaceEX(IndexType cellIdx)
{
    return placer->_db.cell(cellIdx);
}

// EigenVector
// std::vector<std::vector<double>> PAPlace::getGuideGradient()
PROJECT_NAMESPACE_END