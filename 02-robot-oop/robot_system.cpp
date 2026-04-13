// ============================================
// OOP Robot Class System
// Author: Edem Uyimeabasi Linus
// Description:
//   Demonstrates C++ OOP concepts through a
//   robot fleet management system.
//   Includes: classes, inheritance, virtual
//   functions, polymorphism, smart pointers.
// Compile:
//   g++ -std=c++17 -o robot robot_system.cpp
// Run:
//   ./robot
// ============================================

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
// ============================================
// SENSOR CLASS
// A sensor has a name and takes a reading.
// Each robot type will have a different sensor.
// ============================================

class Sensor {
private:
    std::string sensor_name;
    double      noise_level;   // simulated measurement noise

public:
    // Constructor — runs when you create a Sensor object
    Sensor(std::string name, double noise)
        : sensor_name(name), noise_level(noise) {}

    // Read a value — adds simulated noise to true value
    double read(double true_value) const {
        // Simple noise simulation: ±noise_level
        double noise = (noise_level * 2.0)
                     * ((double)rand() / RAND_MAX)
                     - noise_level;
        return true_value + noise;
    }

    std::string name() const { return sensor_name; }
};
// ============================================
// MOTOR CLASS
// Controls movement. Ground robots have wheel
// motors. UAVs have propeller motors.
// ============================================

class Motor {
private:
    std::string motor_type;
    double      max_speed;    // metres per second
    double      current_speed;

public:
    Motor(std::string type, double max_spd)
        : motor_type(type),
          max_speed(max_spd),
          current_speed(0.0) {}

    // Set speed — clamps to valid range
    void set_speed(double speed) {
        // std::max and std::min clamp the value
        current_speed = std::max(0.0,
                        std::min(speed, max_speed));
    }

    double get_speed()  const { return current_speed; }
    double get_max()    const { return max_speed; }
    std::string type()  const { return motor_type; }
};
// ============================================
// ROBOT BASE CLASS
// This is an abstract class — you cannot
// create a Robot directly. You can only create
// GroundRobot or UAV (the subclasses).
//
// "= 0" makes a function PURE VIRTUAL —
// meaning every subclass MUST implement it.
// ============================================

class Robot {
protected:
    // "protected" means subclasses can access
    // these — but outside code cannot
    std::string name;
    double      battery_pct;
    double      x_pos, y_pos;   // position in metres
    Sensor      sensor;
    Motor       motor;

public:
    // Constructor
    Robot(std::string n,
          double      bat,
          Sensor      s,
          Motor       m)
        : name(n),
          battery_pct(bat),
          x_pos(0.0), y_pos(0.0),
          sensor(s),
          motor(m) {}

    // ── Pure virtual functions ───────────────
    // Every subclass MUST implement these
    virtual void        move(double speed)     = 0;
    virtual void        sense()                = 0;
    virtual std::string type()           const = 0;
    virtual double      battery_drain(double speed) const = 0;

    // ── Concrete functions ───────────────────
    // These are shared by ALL robot types
    // Subclasses inherit these automatically

    void status() const {
        std::cout << "\n[" << type() << "] "
                  << name << "\n"
                  << "  Battery  : " << battery_pct << "%\n"
                  << "  Position : ("
                  << x_pos << ", " << y_pos << ") m\n"
                  << "  Sensor   : " << sensor.name() << "\n"
                  << "  Motor    : " << motor.type()
                  << " (max " << motor.get_max() << " m/s)\n";
    }

    void charge(double amount) {
        battery_pct = std::min(100.0, battery_pct + amount);
        std::cout << name << " charged to "
                  << battery_pct << "%\n";
    }

    bool is_alive() const { return battery_pct > 0.0; }

    std::string get_name() const { return name; }

    // Virtual destructor — important for
    // classes that will be inherited
    virtual ~Robot() = default;
};
// ============================================
// GROUNDROBOT SUBCLASS
// Inherits everything from Robot.
// Adds: wheel radius, tracks distance travelled.
// Overrides: move(), sense(), type(),
//            battery_drain()
// ============================================

class GroundRobot : public Robot {
private:
    double wheel_radius;     // metres
    double distance_travelled;

public:
    GroundRobot(std::string name,
                double      battery,
                double      wheel_r)
        : Robot(name,
                battery,
                Sensor("HC-SR04 Ultrasonic", 0.5),
                Motor("DC Wheel Motor", 3.0)),
          wheel_radius(wheel_r),
          distance_travelled(0.0) {}

    // "override" tells compiler this replaces
    // the base class virtual function
    void move(double speed) override {
        if (!is_alive()) {
            std::cout << name << " is out of battery!\n";
            return;
        }
        motor.set_speed(speed);
        double actual_speed = motor.get_speed();

        // Update position — moving in X direction
        double dt = 0.1;   // 100ms timestep
        x_pos    += actual_speed * dt;
        distance_travelled += actual_speed * dt;

        // Drain battery
        battery_pct -= battery_drain(actual_speed);
        battery_pct  = std::max(0.0, battery_pct);

        std::cout << name << " rolling at "
                  << actual_speed << " m/s | "
                  << "Pos: (" << x_pos << ", "
                  << y_pos << ") m\n";
    }

    void sense() override {
        double raw  = sensor.read(x_pos + 2.0);
        std::cout << name << " ultrasonic reading: "
                  << raw << " m ahead\n";
    }

    double battery_drain(double speed) const override {
        return speed * 0.05;  // 0.05% per m/s
    }

    std::string type() const override {
        return "GroundRobot";
    }

    double get_distance() const {
        return distance_travelled;
    }
};
// ============================================
// UAV SUBCLASS
// Inherits from Robot.
// Adds: altitude tracking, flight modes.
// Overrides: move(), sense(), type(),
//            battery_drain()
// ============================================

class UAV : public Robot {
private:
    double altitude;         // metres above ground
    double target_altitude;

public:
    UAV(std::string name, double battery)
        : Robot(name,
                battery,
                Sensor("GPS + IMU", 0.1),
                Motor("Brushless Propeller", 15.0)),
          altitude(0.0),
          target_altitude(0.0) {}

    void set_target_altitude(double alt) {
        target_altitude = alt;
        std::cout << name << " target altitude set to "
                  << alt << " m\n";
    }

    void move(double speed) override {
        if (!is_alive()) {
            std::cout << name << " is out of battery!\n";
            return;
        }
        motor.set_speed(speed);
        double actual_speed = motor.get_speed();

        // Update altitude toward target
        double dt  = 0.1;
        double err = target_altitude - altitude;

        // Simple proportional climb/descent
        if (std::abs(err) > 0.1) {
            altitude += (err > 0 ? 1 : -1)
                      * actual_speed * dt;
        }

        // Move horizontally
        x_pos += actual_speed * dt * 0.7;

        battery_pct -= battery_drain(actual_speed);
        battery_pct  = std::max(0.0, battery_pct);

        std::cout << name << " flying at "
                  << actual_speed << " m/s | "
                  << "Alt: " << altitude << " m | "
                  << "Pos: (" << x_pos << ", "
                  << y_pos << ") m\n";
    }

    void sense() override {
        double gps_x = sensor.read(x_pos);
        double gps_y = sensor.read(y_pos);
        std::cout << name << " GPS reading: ("
                  << gps_x << ", " << gps_y << ") m | "
                  << "Alt: " << altitude << " m\n";
    }

    double battery_drain(double speed) const override {
        // UAVs drain 4x more battery than ground robots
        return speed * 0.20;
    }

    std::string type() const override { return "UAV"; }

    double get_altitude() const { return altitude; }
};
// ============================================
// MAIN FUNCTION
// Demonstrates polymorphism:
// A vector of Robot pointers holds different
// robot types. Calling move() on each calls
// the correct version automatically.
// ============================================

int main() {
    std::srand(42);  // fixed seed for reproducible noise

    // Create a fleet using smart pointers
    // unique_ptr auto-deletes memory when done
    std::vector<std::unique_ptr<Robot>> fleet;

    fleet.push_back(
        std::make_unique<GroundRobot>("Rover-1", 100.0, 0.05));

    fleet.push_back(
        std::make_unique<GroundRobot>("Rover-2", 80.0, 0.06));

    auto uav = std::make_unique<UAV>("Hawk-1", 100.0);
    uav->set_target_altitude(50.0);
    fleet.push_back(std::move(uav));

    // ── Show initial status ──────────────────
    std::cout << "=== FLEET STATUS ===\n";
    for (auto& robot : fleet) {
        robot->status();
    }

    // ── Run 5 timesteps ─────────────────────
    std::cout << "\n=== SIMULATION START ===\n";
    for (int step = 1; step <= 5; step++) {
        std::cout << "\n--- Step " << step << " ---\n";
        for (auto& robot : fleet) {
            if (robot->is_alive()) {
                robot->move(2.0);
                robot->sense();
            }
        }
    }

    // ── Final status ─────────────────────────
    std::cout << "\n=== FINAL STATUS ===\n";
    for (auto& robot : fleet) {
        robot->status();
    }

    return 0;
}
