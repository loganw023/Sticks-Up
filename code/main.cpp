#include <Arduino.h>
#include <Servo.h>
#include <AccelStepper.h>


// =====================================================
// PLAYER 1 BUTTONS
// =====================================================

#define P1_FORWARD_BUTTON 17
#define P1_BACK_BUTTON    23
#define P1_ATTACK_BUTTON  16



// =====================================================
// PLAYER 2 BUTTONS
// =====================================================

#define P2_FORWARD_BUTTON 25
#define P2_BACK_BUTTON    27
#define P2_ATTACK_BUTTON  29



// =====================================================
// POTENTIOMETERS
// =====================================================

#define PLAYER1_POT A3
#define PLAYER2_POT A4



// =====================================================
// PLAYER 1 Y AXIS
// =====================================================

#define Y_STEP_PIN    60
#define Y_DIR_PIN     61
#define Y_ENABLE_PIN  56


AccelStepper player1Stepper(
    AccelStepper::DRIVER,
    Y_STEP_PIN,
    Y_DIR_PIN
);



// =====================================================
// PLAYER 2 X AXIS
// =====================================================

#define X_STEP_PIN    46
#define X_DIR_PIN     48
#define X_ENABLE_PIN  62


AccelStepper player2Stepper(
    AccelStepper::DRIVER,
    X_STEP_PIN,
    X_DIR_PIN
);



// =====================================================
// SERVOS
// =====================================================

#define P1_WAIST_SERVO 45
#define P1_SWORD_SERVO 47

#define P2_WAIST_SERVO 41
#define P2_SWORD_SERVO 43


Servo p1Waist;
Servo p1Sword;

Servo p2Waist;
Servo p2Sword;



// =====================================================
// POSITION SETTINGS
// =====================================================

long minPosition = 0;
long maxPosition = 5000;

const long POSITION_DEADBAND = 10;



// =====================================================
// ACTION STATES
// =====================================================

// PLAYER 1

bool p1Attacking = false;
unsigned long p1AttackTimer = 0;

bool p1Leaning = false;
unsigned long p1LeanTimer = 0;



// PLAYER 2

bool p2Attacking = false;
unsigned long p2AttackTimer = 0;

bool p2Leaning = false;
unsigned long p2LeanTimer = 0;



const unsigned long attackTime = 500;
const unsigned long leanTime = 300;



// =====================================================
// FUNCTION PROTOTYPES
// =====================================================

void readPlayer1Movement();
void readPlayer2Movement();

void player1Actions();
void player2Actions();

void updateActions();

void p1LeanForward();
void p1LeanBack();
void p1Attack();

void p2LeanForward();
void p2LeanBack();
void p2Attack();



// =====================================================
// SETUP
// =====================================================

void setup()
{

    Serial.begin(115200);


    Serial.println();
    Serial.println("--------------------------------");
    Serial.println(" TWO PLAYER FIGHT CONTROLLER ");
    Serial.println("--------------------------------");
    Serial.println("P1: Y Axis | Waist 45 | Sword 47");
    Serial.println("P2: X Axis | Waist 41 | Sword 43");
    Serial.println("--------------------------------");



    pinMode(P1_FORWARD_BUTTON, INPUT_PULLUP);
    pinMode(P1_BACK_BUTTON, INPUT_PULLUP);
    pinMode(P1_ATTACK_BUTTON, INPUT_PULLUP);


    pinMode(P2_FORWARD_BUTTON, INPUT_PULLUP);
    pinMode(P2_BACK_BUTTON, INPUT_PULLUP);
    pinMode(P2_ATTACK_BUTTON, INPUT_PULLUP);



  pinMode(Y_ENABLE_PIN, OUTPUT);
  pinMode(X_ENABLE_PIN, OUTPUT);

  digitalWrite(Y_ENABLE_PIN, LOW);
  digitalWrite(X_ENABLE_PIN, LOW);



    player1Stepper.setMaxSpeed(5000);
    player1Stepper.setAcceleration(5000);


    player2Stepper.setMaxSpeed(5000);
    player2Stepper.setAcceleration(5000);



    p1Waist.attach(P1_WAIST_SERVO);
    p1Sword.attach(P1_SWORD_SERVO);

    p2Waist.attach(P2_WAIST_SERVO);
    p2Sword.attach(P2_SWORD_SERVO);



    p1Waist.write(90);
    p1Sword.write(0);

    p2Waist.write(90);
    p2Sword.write(0);



    player1Stepper.setCurrentPosition(0);
    player2Stepper.setCurrentPosition(0);



    Serial.println("System Ready");

}



// =====================================================
// LOOP
// =====================================================

void loop()
{

    player1Stepper.run();

    player2Stepper.run();



    readPlayer1Movement();

    readPlayer2Movement();



    player1Actions();

    player2Actions();



    updateActions();

}



// =====================================================
// PLAYER MOVEMENT
// =====================================================

void readPlayer1Movement()
{

    static long lastTarget = -99999;


    int value = analogRead(PLAYER1_POT);


    long target = map(
        value,
        0,
        1023,
        maxPosition,
        minPosition
    );


    if(abs(target - lastTarget) > POSITION_DEADBAND)
    {

        Serial.print("P1 Position: ");
        Serial.print(target);
        Serial.print("  Pot: ");
        Serial.println(value);


        player1Stepper.moveTo(target);

        lastTarget = target;

    }

}



void readPlayer2Movement()
{

    static long lastTarget = -99999;


    int value = analogRead(PLAYER2_POT);


    long target = map(
        value,
        0,
        1023,
        maxPosition,
        minPosition
    );


    if(abs(target - lastTarget) > POSITION_DEADBAND)
    {

        Serial.print("P2 Position: ");
        Serial.print(target);
        Serial.print("  Pot: ");
        Serial.println(value);


        player2Stepper.moveTo(target);

        lastTarget = target;

    }

}



// =====================================================
// BUTTON INPUTS
// =====================================================

void player1Actions()
{

    if(digitalRead(P1_FORWARD_BUTTON) == LOW)
        p1LeanForward();


    if(digitalRead(P1_BACK_BUTTON) == LOW)
        p1LeanBack();


    if(digitalRead(P1_ATTACK_BUTTON) == LOW)
        p1Attack();

}



void player2Actions()
{

    if(digitalRead(P2_FORWARD_BUTTON) == LOW)
        p2LeanForward();


    if(digitalRead(P2_BACK_BUTTON) == LOW)
        p2LeanBack();


    if(digitalRead(P2_ATTACK_BUTTON) == LOW)
        p2Attack();

}



// =====================================================
// PLAYER 1 ACTIONS
// =====================================================

void p1LeanForward()
{

    if(p1Leaning)
        return;


    Serial.println("P1 Forward Lean");


    p1Waist.write(20);

    p1LeanTimer = millis();

    p1Leaning = true;

}



void p1LeanBack()
{

    if(p1Leaning)
        return;


    Serial.println("P1 Back Lean");


    p1Waist.write(160);

    p1LeanTimer = millis();

    p1Leaning = true;

}



void p1Attack()
{

    if(p1Attacking)
        return;


    Serial.println("P1 Attack");


    p1Sword.write(95);

    p1AttackTimer = millis();

    p1Attacking = true;

}



// =====================================================
// PLAYER 2 ACTIONS
// =====================================================

void p2LeanForward()
{

    if(p2Leaning)
        return;


    Serial.println("P2 Forward Lean");


    p2Waist.write(20);

    p2LeanTimer = millis();

    p2Leaning = true;

}



void p2LeanBack()
{

    if(p2Leaning)
        return;


    Serial.println("P2 Back Lean");


    p2Waist.write(160);

    p2LeanTimer = millis();

    p2Leaning = true;

}



void p2Attack()
{

    if(p2Attacking)
        return;


    Serial.println("P2 Attack");


    p2Sword.write(95);

    p2AttackTimer = millis();

    p2Attacking = true;

}



// =====================================================
// TIMER UPDATE
// =====================================================

void updateActions()
{

    if(p1Attacking && millis() - p1AttackTimer >= attackTime)
    {

        p1Sword.write(0);

        Serial.println("P1 Sword Reset");

        p1Attacking = false;

    }



    if(p2Attacking && millis() - p2AttackTimer >= attackTime)
    {

        p2Sword.write(0);

        Serial.println("P2 Sword Reset");

        p2Attacking = false;

    }



    if(p1Leaning && millis() - p1LeanTimer >= leanTime)
    {

        p1Waist.write(90);

        Serial.println("P1 Waist Reset");

        p1Leaning = false;

    }



    if(p2Leaning && millis() - p2LeanTimer >= leanTime)
    {

        p2Waist.write(90);

        Serial.println("P2 Waist Reset");

        p2Leaning = false;

    }

}
