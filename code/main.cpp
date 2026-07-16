#include <Arduino.h>
#include <Servo.h>
#include <AccelStepper.h>


// =====================================================
// BUTTONS
// =====================================================

#define FORWARD_BUTTON 17
#define BACK_BUTTON    23
#define ATTACK_BUTTON  16



// =====================================================
// POTENTIOMETER
// =====================================================

#define PLAYER_POT A3



// =====================================================
// RAMPS Y AXIS - PLAYER
// =====================================================

#define Y_STEP_PIN    60
#define Y_DIR_PIN     61
#define Y_ENABLE_PIN  56


AccelStepper yStepper(
    AccelStepper::DRIVER,
    Y_STEP_PIN,
    Y_DIR_PIN
);



// =====================================================
// RAMPS X AXIS - ENEMY
// =====================================================

#define X_STEP_PIN    54
#define X_DIR_PIN     55


AccelStepper xStepper(
    AccelStepper::DRIVER,
    X_STEP_PIN,
    X_DIR_PIN
);



// =====================================================
// SERVOS
// =====================================================

#define WAIST_SERVO_PIN 45
#define SWORD_SERVO_PIN 47


Servo waist;
Servo sword;



// =====================================================
// Y SETTINGS - PLAYER
// =====================================================

long minPosition = 0;
long maxPosition = 5000;

const long POSITION_DEADBAND = 10;



// =====================================================
// X SETTINGS - ENEMY
// =====================================================

long enemyMinPosition = 0;
long enemyMaxPosition = 5000;

bool enemyMovingForward = true;



// =====================================================
// ACTION STATE
// =====================================================

bool attacking = false;

unsigned long attackTimer = 0;

const unsigned long attackTime = 500;



bool leaning = false;

unsigned long leanTimer = 0;

const unsigned long leanTime = 300;



// =====================================================
// FUNCTION PROTOTYPES
// =====================================================

void readMovement();

void leanForward();
void leanBack();
void attack();

void updateActions();

void homeEnemy();

void updateEnemyMovement();



// =====================================================
// SETUP
// =====================================================

void setup()
{

    Serial.begin(115200);


    pinMode(FORWARD_BUTTON, INPUT_PULLUP);
    pinMode(BACK_BUTTON, INPUT_PULLUP);
    pinMode(ATTACK_BUTTON, INPUT_PULLUP);



    pinMode(Y_ENABLE_PIN, OUTPUT);
    digitalWrite(Y_ENABLE_PIN, LOW);



    yStepper.setMaxSpeed(5000);
    yStepper.setAcceleration(5000);



    xStepper.setMaxSpeed(5000);
    xStepper.setAcceleration(5000);



    waist.attach(WAIST_SERVO_PIN);
    sword.attach(SWORD_SERVO_PIN);


    waist.write(90);
    sword.write(0);



    homeEnemy();



    Serial.println("Controller ready");

}



// =====================================================
// LOOP
// =====================================================

void loop()
{

    // motors must run constantly

    yStepper.run();

    xStepper.run();



    readMovement();


    updateEnemyMovement();


    updateActions();



    if(digitalRead(BACK_BUTTON) == LOW)
    {
        leanForward();
    }



    if(digitalRead(FORWARD_BUTTON) == LOW)
    {
        leanBack();
    }



    if(digitalRead(ATTACK_BUTTON) == LOW)
    {
        attack();
    }

}



// =====================================================
// PLAYER POT -> Y STEPPER
// =====================================================

void readMovement()
{

    static long lastTarget = -99999;


    int potValue = analogRead(PLAYER_POT);



    long target = map(
        potValue,
        0,
        1023,
        maxPosition,
        minPosition
    );



    if(abs(target - lastTarget) > POSITION_DEADBAND)
    {

        yStepper.moveTo(target);

        lastTarget = target;

    }

}



// =====================================================
// ENEMY HOME
// =====================================================

void homeEnemy()
{

    Serial.println("Homing enemy");


    xStepper.setCurrentPosition(0);

    xStepper.moveTo(0);



    while(xStepper.distanceToGo() != 0)
    {
        xStepper.run();
    }



    Serial.println("Enemy ready");

}



// =====================================================
// ENEMY WALKING
// =====================================================

void updateEnemyMovement()
{

    // wait until enemy reaches destination

    if(xStepper.distanceToGo() != 0)
        return;



    if(enemyMovingForward)
    {
        xStepper.moveTo(enemyMaxPosition);
    }
    else
    {
        xStepper.moveTo(enemyMinPosition);
    }



    enemyMovingForward = !enemyMovingForward;

}



// =====================================================
// UPDATE NON-BLOCKING ACTIONS
// =====================================================

void updateActions()
{

    if(attacking)
    {

        if(millis() - attackTimer >= attackTime)
        {

            sword.write(0);

            attacking = false;

        }

    }



    if(leaning)
    {

        if(millis() - leanTimer >= leanTime)
        {

            waist.write(90);

            leaning = false;

        }

    }

}



// =====================================================
// WAIST
// =====================================================

void leanForward()
{

    if(leaning)
        return;


    Serial.println("Forward lean");


    waist.write(20);


    leanTimer = millis();

    leaning = true;

}



void leanBack()
{

    if(leaning)
        return;


    Serial.println("Back lean");


    waist.write(160);


    leanTimer = millis();

    leaning = true;

}



// =====================================================
// ATTACK
// =====================================================

void attack()
{

    if(attacking)
        return;


    Serial.println("Attack");


    sword.write(70);


    attackTimer = millis();

    attacking = true;

}
