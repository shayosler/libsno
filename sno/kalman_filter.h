/**
 * @class Kalman_filter
 * @brief Kalman filter implementation
 *
 * Assumes constant process noise (Q)
 *
 */

#ifndef SO_KALMAN_FILTER_H
#define SO_KALMAN_FILTER_H
#include <stddef.h>
#include <functional>

#include <eigen3/Eigen/Dense>

#include <sno/time_utils.h>

namespace so
{

template<int N, int M = N>
class Kalman_filter
{
public:
  typedef Eigen::Matrix<double, N, N> MatrixNN;
  typedef Eigen::Matrix<double, 1, N> Matrix1N;
  typedef Eigen::Matrix<double, N, 1> MatrixN1;
  typedef Eigen::Matrix<double, N, M> MatrixNM;
  typedef Eigen::Matrix<double, M, 1> MatrixM1;

public:
  /**
   * @brief Constructor, a new Kalman filter.
   * @param A State transition matrix, NxN
   * @param B Control input model, NxN      //TODO: NxM?
   * @param Q Process noise covariance, NxN
   * @param x0 Initial system state, Nx1
   * @param P0 Initial error covariance, NxN
   * @param t0 Initial timestamp
   */
  //TODO: A is usually going to be a function of dt, not just a static matrix
  // Instead of passing in an A matrix pass in a pointer to a function that
  // Takes time as an argument and returns the appropriate A matrix?
  // Make Predict() and Update() take in a timestamp? Or dt?
  Kalman_filter(const MatrixNN& A,
                const MatrixNM& B,
                const MatrixNN& Q,
                const MatrixN1& x0,
                const MatrixNN& P0,
                const double t0)
    :
      m_A([A](const double){return A;}),
      m_B([B](const double){return B;}),
      m_Q(Q),
      m_x(x0),
      m_P(P0),
      m_last_timestamp(t0)
  {

  }

  /**
   * @brief Predict the system state based on the current state
   * @param Current timestamp
   */
  void Predict(const double t)
  {
    MatrixM1 u = MatrixM1::Zero();
    Predict(u, t);
  }

  /**
   * @brief Predict the system state based on the current state and the current
   * inputs, u
   * @param u Current inputs to the system, Mx1
   * @param t Current timestamp
   */
  void Predict(const MatrixM1& u, const double t)
  {
    double dt = t - m_last_timestamp;
    m_last_timestamp = t;
    MatrixNN A = m_A(dt);
    MatrixNM B = m_B(dt);
    m_x = A * m_x + B * u;
    m_P = A * m_P * A.transpose() + m_Q;
  }

  /**
   * @brief Update the current state estimate based on a single observation, z.
   * z = Hx + v, where x is the true system state, H is the mapping from true
   * state space to observed state space and v is observation error sampled
   * from a 0 mean normal distribution with covariance R. v~N(0,R). Predict()
   * function should be called to propagate the estimated solution up to the
   * current timestamp before update.
   * @param z Observation, scalar
   * @param H Observation model, maps true state space to observed state space, 1xN
   * @param R Observation error covariance, scalar
   */
  void Update(const double z,
              const Matrix1N& H,
              const double R)
  {
    Update(Eigen::Matrix<double, 1, 1>(z),
           H,
           Eigen::Matrix<double, 1, 1>(R));
  }

  /**
   * @brief Update the current state estimate based on a vector of observations,
   * z. z = Hx + v, where x is the true system state, H is the mapping from true
   * state space to observed state space and v is observation error sampled
   * from a 0 mean normal distribution with covariance R. v~N(0,R). Predict()
   * function should be called to propagate the estimated solution up to the
   * current timestamp before update.
   * @param z Observation, Ux1
   * @param H Observation model, maps true state space to observed state space, UxN
   * @param R Observation error covariance, UxU
   */
  template<int U>
  void Update(const Eigen::Matrix<double, U, 1> z,
              const Eigen::Matrix<double, U, N>& H,
              const Eigen::Matrix<double, U, U>& R)
  {
    // Innovation
    Eigen::Matrix<double, U, 1> y = z - H * m_x;

    // Innovation covariance
    Eigen::Matrix<double, U, U> S = H * m_P * H.transpose() + R;

    // Optimal Kalman gain
    const Eigen::Matrix<double, N, U> K = m_P * H.transpose() * S.inverse();

    // Update state estimate
    m_x = m_x + K * y;

    // Update error covariance
    m_P = (MatrixNN::Identity() - K*H) * m_P;

    // Joseph form of error covariance, valid for any Kalman gain
//    m_P = (MatrixNN::Identity() - K*H) * m_P *
//          (MatrixNN::Identity() - K*H).eval().transpose() +
//          K * R * K.transpose();

    //TODO: update last timestamp here?
  }

  /**
   * @brief Get the current state estimate (x)
   * @return Current state estimate
   */
  MatrixN1 Get_state_estimate() const {return m_x;}

  /**
   * @brief Get the current error covariance (P)
   * @return Current error covariance
   */
  MatrixNN Get_error_covariance() const {return m_P;}

  void Set_state_estimate(const MatrixNN& x)
  {
    //TODO: update time
    m_x = x;
  }

  void Set_error_covariance(const MatrixNN& p)
  {
    m_P = p;
  }

private:

  /**
   * @brief m_A State transition matrix. Defines how the state changes based on
   * current state
   */
//  MatrixNN m_A;

  /**
   * @brief m_B Control input matrix. Defines how the state changes based on
   * control inputs
   */
//  MatrixNN m_B;

  /**
   * @brief Function that takes dT as an input and returns the state transition
   * matrix for the given dT
   * @return State transition (A) matrix for the given dT
   */
  std::function<MatrixNN (const double)> m_A;

  /**
   * @brief Function that takes dT as an input and returns the control input
   * matrix for the given dT
   * @return Control input (B) matrix for the given dT
   */
  std::function<MatrixNM (const double)> m_B;

  /**
   * @brief m_Q Process noise covariance matrix
   */
  MatrixNN m_Q;

  /**
   * @brief x Current system state
   */
  MatrixN1 m_x;

  /**
   * @brief P Current estimate error covariance matrix
   */
  MatrixNN m_P;

  /**
   * @brief m_last_timestamp Last time that the current state estimate was
   * revised
   */
  double m_last_timestamp;

};
} //namespace so

#endif
