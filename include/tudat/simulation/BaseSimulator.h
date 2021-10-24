/*    Copyright (c) 2010-2019, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#ifndef TUDAT_BASESIMULATOR_H
#define TUDAT_BASESIMULATOR_H

#include <tudat/simulation/helpers.h>

#include <boost/make_shared.hpp>
#include <chrono>
#include <string>
#include <vector>

#include "tudat/astro/ephemerides/frameManager.h"
#include "tudat/astro/propagators/dynamicsStateDerivativeModel.h"
#include "tudat/astro/propagators/integrateEquations.h"
#include "tudat/astro/propagators/nBodyStateDerivative.h"
#include "tudat/basics/tudatTypeTraits.h"
#include "tudat/basics/utilities.h"
#include "tudat/math/interpolators/lagrangeInterpolator.h"
#include "tudat/simulation/propagation_setup/createEnvironmentUpdater.h"
#include "tudat/simulation/propagation_setup/createStateDerivativeModel.h"
#include "tudat/simulation/propagation_setup/propagationSettings.h"
#include "tudat/simulation/propagation_setup/propagationTermination.h"
#include "tudat/simulation/propagation_setup/setNumericallyIntegratedStates.h"

namespace tudat {

namespace numerical_simulation {

using namespace propagators;
using namespace simulation_setup;

//! @get_docstring(BaseSimulator)
template <typename StateScalarType = double, typename TimeType = double,
          typename std::enable_if<
              is_state_scalar_and_time_type<StateScalarType, TimeType>::value,
              int>::type = 0>
class BaseSimulator : public std::enable_shared_from_this<
                          BaseSimulator<StateScalarType, TimeType>> {
 public:
  using InitialGlobalStateType =
      Eigen::Matrix<StateScalarType, Eigen::Dynamic, Eigen::Dynamic>;
  using NumericalSolutionBaseType = std::vector<
      std::map<TimeType, Eigen::Matrix<StateScalarType, Eigen::Dynamic, 1>>>;
  using DependentNumericalSolutionBaseType =
      std::vector<std::map<TimeType, Eigen::VectorXd>>;

  //! @get_docstring(BaseSimulator.ctor)
  explicit BaseSimulator(const SystemOfBodies &bodies,
                         const bool clearNumericalSolutions = true,
                         const bool setIntegratedResult = true)
      : bodies_(bodies),
        clearNumericalSolutions_(clearNumericalSolutions),
        setIntegratedResult_(setIntegratedResult) {}

  //! @get_docstring(BaseSimulator.destructor)
  virtual ~BaseSimulator() = default;

  //  virtual std::shared_ptr<IntegrationInterface> getIntegrationInterface();

  //  virtual std::shared_ptr<DependentsObserver> getDependentsObserver();

  //  virtual std::shared_ptr<ComputationExpense> getComputationTimeObserver();

  //  virtual std::shared_ptr<SolutionObserver> getSolutionObserver();
  //
  //  virtual std::shared_ptr<FunctionEvalObserver> getFunctionEvalObserver();
  //
  //  virtual std::shared_ptr<TerminationObserver> getTerminationObserver();

  //! @get_docstring(BaseSimulator.integrateEquationsOfMotion)
  [[deprecated]] virtual void integrateEquationsOfMotion() = 0;

  //! @get_docstring(BaseSimulator.integrationCompletedSuccessfully)
  virtual bool integrationCompletedSuccessfully() const = 0;

  //! @get_docstring(BaseSimulator.getEquationsOfMotionNumericalSolutionBase)
  virtual NumericalSolutionBaseType
  getEquationsOfMotionNumericalSolutionBase() = 0;

  //! @get_docstring(BaseSimulator.getDependentVariableNumericalSolutionBase)
  [[deprecated]] virtual DependentNumericalSolutionBaseType
  getDependentVariableNumericalSolutionBase() = 0;

  //! @get_docstring(BaseSimulator.getCumulativeComputationTimeHistoryBase)
  [[deprecated]] virtual std::vector<std::map<TimeType, double>>
  getCumulativeComputationTimeHistoryBase() = 0;

  //! @get_docstring(BaseSimulator.getSystemOfBodies)
  SystemOfBodies getSystemOfBodies() { return bodies_; }

  //! @get_docstring(BaseSimulator.resetSystemOfBodies)
  void resetSystemOfBodies(const SystemOfBodies &bodies) { bodies_ = bodies; }

  //! @get_docstring(BaseSimulator.getSetIntegratedResult)
  bool getSetIntegratedResult() { return setIntegratedResult_; }

  //! @get_docstring(BaseSimulator.resetSetIntegratedResult)
  void resetSetIntegratedResult(const bool setIntegratedResult) {
    setIntegratedResult_ = setIntegratedResult;
  }

  //! @get_docstring(BaseSimulator.processNumericalEquationsOfMotionSolution)
  virtual void processNumericalEquationsOfMotionSolution() = 0;

 protected:
  //! @get_docstring(BaseSimulator.bodies_)
  SystemOfBodies bodies_;

  //! @get_docstring(BaseSimulator.clearNumericalSolutions_)
  bool clearNumericalSolutions_;

  //! @get_docstring(BaseSimulator.setIntegratedResult_)
  bool setIntegratedResult_;

  //  template <typename Derived>
  //  std::shared_ptr<Derived> shared_from_base() {
  //    return std::static_pointer_cast<Derived>(shared_from_this());
  //  }
};

}  // namespace numerical_simulation

}  // namespace tudat

#endif  // TUDAT_BASESIMULATOR_H
