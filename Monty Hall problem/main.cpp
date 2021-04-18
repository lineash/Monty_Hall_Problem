//
//  main.cpp
//  Monty Hall problem
//
//  Created by line.ash on 2021/04/17.
//
//====================================================================================
//전처리기
#define _CRT_SECURE_NO_WARNINGS    // strcpy 보안 경고로 인한 컴파일 에러 방지
#define ANIMATION_TIME 0.01f
#define ANIMATION_STEP 1
#define DOOR_OPENED 1
#define DOOR_CLOSED 0

#include <bangtal.h>
#include <bangtal>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cmath>
#include <math.h>

//====================================================================================
//변수선언

SceneID scene[30];
//SceneID scene1, scene2, scene3, scene4, scene5, scene6, scene7, scene8, scene9;
ObjectID start, next, script, yes, nope, yes1, endgame;
SoundID sound1, sound2, sound3, sound4;
TimerID startbgm, scene4Count = 0, Trigger, scene4Total = 0, TotalTrigger, openGoatTimer, simulTimer;
int page = 0;
int script_count = 0;
int selectDoor = 0;
int openGoat = 0;
int total = 1;
int win = 0;
int simulNum = 0;
float winrate = 0;
bool doorClick = false;
bool change = false;
bool fin = false;

//구조체 선언
struct doorS
{
    ObjectID door;
    bool opend = false; //opened로 표기하려 했으나 실수함.
    bool car = false;
    bool choose = false;
};

struct doorS doorS[10000];

//====================================================================================
//오브젝트 생성

ObjectID createObject(const char* image, SceneID scene, int x, int y, bool shown = true){
    ObjectID object1 = createObject(image);
    locateObject(object1, scene, x, y);
    if(shown) showObject(object1);
    
    return object1;
}
//====================================================================================
//doorS 상태 변경 함수 //안씀
void carDoor(struct doorS *d)
{
    d->car = true;
}

void openDoor(struct doorS *d)
{
    d->opend = true;
}
//====================================================================================

void create3Door(SceneID scene) //3도어 생성
{
    doorS[0].door = createObject("door_closed.png", scene, 300, 200, true);
    scaleObject(doorS[0].door, 0.5);
    doorS[1].door = createObject("door_closed.png", scene, 730, 200, true);
    scaleObject(doorS[1].door, 0.5);
    doorS[2].door = createObject("door_closed.png", scene, 1160, 200, true);
    scaleObject(doorS[2].door, 0.5);
    doorS[rand()%3].car = true; //랜덤으로 당첨 문 설정
}
void create100Door(SceneID scene) //3도어 생성
{
    for(int i = 0; i < 100; i++) //초기화
    {
        doorS[i].car = false;
        doorS[i].choose = false;
        doorS[i].opend = false;
    }
    for(int i = 0; i < 5; i++) //생성
    {
        for(int l = 0; l < 20; l++)
        {
            doorS[20*i+l].door = createObject("door_closed.png", scene, 265 + l*55, 130 + i*130);
            scaleObject(doorS[20*i+l].door, 0.1);
        }
    }
    doorS[rand()%100].car = true; //랜덤으로 당첨 문 설정
}
void doorReset()
{
    for(int i = 0; i < 3; i++)
    {
        doorS[i].car = false;
        doorS[i].choose = false;
        doorS[i].opend = false;
    }
}
void runScript()
{
    char buf[50];
    sprintf(buf, "script%d.png", script_count + 1); //script_count에 맞는 문장 출력
    script_count++;
    setObjectImage(script, buf);
}
void openGoatDoor()
{
    bool simu = true;
    while(simu){
    for(int i = 0; i < 3; i++)
    {
        if(doorS[i].choose == false && doorS[i].car == false && rand()%2 == 1) //순서대로 열지 않기 위해 랜덤 적용
        {
            setObjectImage(doorS[i].door, "door_goat.png");
            doorS[i].opend = true;
            i += 3;
            simu = false;
        }
    }
    }
}
void open100GoatDoor()
{
    int i = rand()%100;
    setTimer(openGoatTimer, ANIMATION_TIME);
    if(doorS[i].opend == false && doorS[i].choose == false && doorS[i].car == false)
    {
        setObjectImage(doorS[i].door, "door_goat.png");
        doorS[i].opend = true;
        openGoat++;
    }
    //startTimer(openGoatTimer);
    if(openGoat == 98) //값이 98이기 때문에 내가 선택한 문이 당첨이더라도 임의의 다른 문 하나는 열리지 않음.
    {
        setObjectImage(script, "change.png");
        showObject(script);
        hideObject(next);
        yes = createObject("yes.png", scene[7], 850, 720);
        scaleObject(yes, 0.3);
        nope = createObject("no.png", scene[7], 950, 720);
        scaleObject(nope, 0.3);
        page++;
    }else{
        startTimer(openGoatTimer);
    }
}
void OopenAnswer(int n) //몬티홀을 따랐을때 정답공개
{
    for(int i = 0; i<n; i++) //정답공개
    {
        if(doorS[i].opend == false && doorS[i].choose == true) //도어가 닫혀 있고 선택한 도어이면
        {
            if(doorS[i].car == true) //당첨이면
            {
                setObjectImage(doorS[i].door, "door_car.png"); //스포츠카 공개
                setObjectImage(script, "success.png"); //바꿔서 맞았으므로 몬티홀이 맞았다.
                startTimer(Trigger); //점수 증가
                if(page == 20 || page == 21) win++;
                //showTimer(scene4Count);
            }
            if(doorS[i].car == false)//꽝이면
            {
                setObjectImage(doorS[i].door, "door_goat.png");
                setObjectImage(script, "fail.png"); //바꿔서 틀렸으므로 몬티홀이 틀렸다.
            }
        }
    }
}
void XopenAnswer(int n) //몬티홀을 따르지 않았을때 정답공개
{
    for(int i = 0; i<n; i++) //정답공개
    {
        if(doorS[i].opend == false && doorS[i].choose == true) //도어가 닫혀 있고 선택한 도어이면
        {
            if(doorS[i].car == true) //당첨이면
            {
                setObjectImage(doorS[i].door, "door_car.png"); //스포츠카 공개
                setObjectImage(script, "fail.png"); //바꾸지 않았는데 맞췄으므로 몬티홀이 틀렸다.
                //startTimer(Trigger); //점수 증가
                //showTimer(scene4Count);
            }
            if(doorS[i].car == false)//꽝이면
            {
                setObjectImage(doorS[i].door, "door_goat.png");
                setObjectImage(script, "success.png"); //바꾸지 않았는데 틀렸으므로 몬티홀이 맞았다.
                startTimer(Trigger);
                //showTimer(scene4Count);
            }
        }
    }
}
void cancelDoor(int n)
{
    doorS[selectDoor].choose = false; //선택했던 도어 선택 취소
    for(int i = 0; i<n; i++)
    {
        if(i!=selectDoor && doorS[i].opend == false) //닫혀있는 나머지 도어를 선택
            doorS[i].choose = true;
    }
}
void chooseRandomDoor()
{
    ObjectID object = doorS[rand()%3].door; //랜덤 선택
    for(int i = 0; i < 3; i++)   //몇번째 문을 선택했는지 검사
    {
        if(object == doorS[i].door)
        {
            doorS[i].choose = true; //doorS[i] 선택 표시
            selectDoor = i; //선택 도어 기록
        }
    }
}
void simulation(int n)
{
    simulNum = n;
    doorReset();
    create3Door(scene[10]);
    chooseRandomDoor();
    openGoatDoor();
    cancelDoor(3);
    OopenAnswer(3);
    setTimer(simulTimer, ANIMATION_TIME);
    startTimer(simulTimer);
}
//====================================================================================

//시작버튼
void mouseCallback(ObjectID object, int x, int y, MouseAction action) //마우스 콜백
{
    if(object == start)
    {
        scene[2] = createScene("scene2", "scene2.png");
        script = createObject("script1.png", scene[2], 550, 600, true);
        script_count++;
        scaleObject(script, 0.5);
        create3Door(scene[2]);
        next = createObject("next.png", scene[2], 760, 560);
        scaleObject(next, 0.3);
        enterScene(scene[2]);
    }
    
//====================================================================================
//다음버튼, 진행
    
    if(object == next)
    {
        if(page == 0)
        {
            runScript();
            //페이지 스크립트 끝. 버튼 숨김. 선택 활성화. 페이지 증가.
            if(script_count == 5)
            {
                hideObject(next);
                doorClick = true; //사용 안하는 중
                page++;
            }
        }
    }
    if(page == 1) //문 선택 페이지
    {
        for(int i = 0; i < 3; i++)   //몇번째 문을 선택했는지 검사
        {
            if(object == doorS[i].door)
            {
                doorS[i].choose = true; //doorS[i] 선택 표시
                char buf[50];
                sprintf(buf, "pick_door_%d.png", i+1);
                setObjectImage(script, buf);
                selectDoor = i; //선택 도어 기록 , 사용 안하는중
                page++;
            }
        }
    }
    if(page == 2) //
    {
        showObject(next);
        if(object == next)
        {
            runScript();
            page++;
            object = 0; //object 초기화. 클릭 없이 바로 넘김 방지
        }
    }
    if(page == 3) //문 공개 페이지
    {
        if(object == next)
        {
            bool simu = true; //선택 시뮬레이션 true
            while(simu){
                for(int i = 0; i < 3; i++)//문 상태 검사
                {
                        if(doorS[i].choose == false)//선택하지 않은 문 인 경우
                        {
                            if(doorS[i].car == false)//자동차 문이 아니면
                            {
                                if(rand()%2 == 1)
                                {//랜덤으로
                                    setObjectImage(doorS[i].door, "door_goat.png"); //염소 문을 열고
                                    doorS[i].opend = true;
                                    simu = false; //시뮬레이션을 끝낸다.
                                    i = 3; //for문 중복 방지
                                    page++; //페이지를 넘긴다.
                                    //object = NULL;
                                }
                            }
                        }
                }
            }
        }
    }
    if(page == 4) //선택을 바꿀 기회
    {
        if(object == next)
        {
            runScript();
            yes = createObject("yes.png", scene[2], 600, 560, true);
            scaleObject(yes, 0.3);
            nope = createObject("no.png", scene[2], 920, 560, true);
            scaleObject(nope, 0.3);
            hideObject(next);
            page++;
            object = 0;
        }
    }
    
    if(page == 5) //바꿀지 선택
    {
        if(object == nope)
        {
            setObjectImage(script, "really.png");
            change = false;
            page++;
            object = 0;
        }
        if(object == yes||yes1)
        {
            setObjectImage(script, "really.png");
            change = true;
            page++;
            object = 0;
        }
    }
    
    if(page == 6) //질문 확인 페이지
    {
        if(object == yes)
        {
            page++;
        }
        if(object == nope)
        {
            page--;
            setObjectImage(script, "script7.png");
        }
    }
    if(page == 7)
    {
        if(change == true)
        {
            for(int i = 0; i < 3; i ++)
            {
                if(doorS[i].opend == false && i != selectDoor)
                {
                    setObjectImage(doorS[i].door, "door_car.png");
                }
            }
            setObjectImage(doorS[selectDoor].door, "door_goat.png");
            setObjectImage(script, "true0.png");
            showObject(next);
            hideObject(yes);
            hideObject(nope);
            page++;
            object = 0;
        }else{
            setObjectImage(doorS[selectDoor].door, "door_goat.png");
            setObjectImage(script, "false0.png");
            showObject(next);
            hideObject(yes);
            hideObject(nope);
            page++;
            object = 0;
        }
    }
    if(page == 8)
    {
        if(object == next)
        {
            scene[3] = createScene("scene3", "scene3.png");
            next = createObject("next.png", scene[3], 1150, 200, true);
            scaleObject(next, 0.4);
            enterScene(scene[3]);
            object = 0;
            page++;
        }
    }
    if(page == 9) //셀프 실험 페이지
    {
        if(object == next)
        {
            scene[4] = createScene("scene4", "scene4.png");
            doorReset(); //도어 리셋
            create3Door(scene[4]);
            script = createObject("script5.png", scene[4], 550, 600, true);
            next = createObject("next.png", scene[4], 760, 560, false);
            scaleObject(script, 0.5);
            scaleObject(next, 0.3);
            enterScene(scene[4]);
        }
        for(int i = 0; i < 3; i++)   //몇번째 문을 선택했는지 검사
        {
            if(object == doorS[i].door)
            {
                doorS[i].choose = true; //doorS[i] 선택 표시
                selectDoor = i; //선택 도어 기록
                page++;
                hideObject(script);
                showObject(next);
            }
        }
    }
    if(page == 10) //양 문 공개
    {
        if(object == next)
        {
            openGoatDoor();
            hideObject(next);
            setObjectImage(script, "script7.png");
            showObject(script);
            yes = createObject("yes.png", scene[4], 600, 560, true);
            scaleObject(yes, 0.3);
            nope = createObject("no.png", scene[4], 920, 560, true);
            scaleObject(nope, 0.3);
            object = 0;
        }
        if(object == yes)
        {
            cancelDoor(3);
            OopenAnswer(3);
            //startTimer(TotalTrigger);
            //showTimer(scene4Total);
            hideObject(yes);
            hideObject(nope);
            showObject(next);
            page++;
            object = 0;
        }
        if(object == nope)
        {
            XopenAnswer(3);
            //startTimer(TotalTrigger);
            //showTimer(scene4Total);
            hideObject(yes);
            hideObject(nope);
            showObject(next);
            page++;
            object = 0;
        }
    }
    if(page == 11)
    {
        if(object == next)
        {
            setObjectImage(script, "retry.png");
            showObject(yes);
            showObject(nope);
            hideObject(next);
        }
        if(object == yes)
        {
            page = 9;
            hideObject(yes);
            hideObject(nope);
            showObject(next);
            object = 0;
        }
        if(object == nope)
        {
            page++;
            hideObject(yes);
            hideObject(nope);
            hideObject(next);
        }
    }
    if(page == 12) //설명
    {
        scene[5] = createScene("scene5", "scene5.png");
        next = createObject("next.png", scene[5], 1150, 200, true);
        scaleObject(next, 0.4);
        enterScene(scene[5]);
        hideTimer();
        object = 0;
        page++;
    }
    if(page == 13)
    {
        if(object == next) //문을 늘려봅시다.
        {
            scene[6] = createScene("scene6", "scene6.png");
            next = createObject("next.png", scene[6], 1150, 200, true);
            scaleObject(next, 0.4);
            enterScene(scene[6]);
            object = 0;
            page++;
        }
    }
    if(page == 14)
    {
        if(object == next) //100개의 문
        {
            scene[7] = createScene("scene7", "scene2.png");
            hideObject(next);
            create100Door(scene[7]); //도어 100개 생성
            script = createObject("100doorC.png", scene[7], 550, 700,true);
            scaleObject(script, 0.5);
            enterScene(scene[7]);
            page++;
        }
    }
    if(page == 15) //선택 페이즈
    {
        for(int i = 0; i < 100; i++)   //몇번째 문을 선택했는지 검사
        {
            if(object == doorS[i].door)
            {
                doorS[i].choose = true; //doorS[i] 선택 표시
                selectDoor = i; //선택 도어 기록
                hideObject(script);
                next = createObject("next.png", scene[7], 750, 750);
                scaleObject(next, 0.3);
                showObject(next);
                object = 0;
                page++;
            }
        }
    }
    if(page == 16)
    {
        if(object == next)
        {
            open100GoatDoor();
        }
    }
    if(page == 17)
    {
        if(object == yes)
        {
            cancelDoor(100);
            OopenAnswer(100);
            hideObject(yes);
            hideObject(nope);
            locateObject(next, scene[7], 850, 720);
            showObject(next);
            page++;
            object = 0;
        }
        if(object == nope)
        {
            XopenAnswer(100);
            hideObject(yes);
            hideObject(nope);
            locateObject(next, scene[7], 850, 720);
            showObject(next);
            page++;
            object = 0;
        }
    }
    if(page == 18)
    {
        if(object == next)
        {
            scene[8] = createScene("scene8", "scene8.png");
            locateObject(next, scene[8], 1150, 200);
            enterScene(scene[8]);
            page++;
            object = 0;
        }
    }
    if(page == 19)
    {
        if(object == next)
        {
            scene[9] = createScene("scene9", "scene9.png");
            locateObject(next, scene[9], 1150, 200);
            enterScene(scene[9]);
            page++;
            object = 0;
        }
        
    }
    if(page == 20)
    {
        if(object == next)
        {
            scene[10] = createScene("scene10", "scene10.png");
            enterScene(scene[10]);
            setTimer(scene4Count, 0);
            showTimer(scene4Count);
            simulation(1000);
            page++;
            locateObject(next, scene[10], 750, 600);
        }
    }
    if(page == 21)
    {
        if(object == next && fin == true)
        {
            winrate = (float)win / 1000;
            char buf[500];
            sprintf(buf, "1000번의 시뮬레이션 결과 선택을 바꿀 경우 %.2f만큼의 성공률이 나왔습니다.", winrate);
            showMessage(buf);
            locateObject(next, scene[10], 750, 250);
            page++;
            stopSound(sound1);
            playSound(sound2, true);
        }
    }
    if(page == 22)
    {
        if(object == next)
        {
            scene[11] = createScene("scene11", "end.png");
            enterScene(scene[11]);
            hideTimer();
            endgame = createObject("next.png", scene[11], 750, 250);
            scaleObject(endgame, 0.3);
        }
        if(object == endgame)
        {
            endGame();
        }
    }
    
}

//=====================================================================================
void timerCallback(TimerID timerF) //타이머 콜백
{
    if(timerF == startbgm)
    {
        playSound(sound1);
    }
    if(timerF == Trigger)
    {
        increaseTimer(scene4Count, 1);
        setTimer(Trigger, 0.01f);
    }
    if(timerF == TotalTrigger)
    {
        increaseTimer(scene4Total, 1);
        setTimer(TotalTrigger, 0.01f);
    }
    if(timerF == openGoatTimer)
    {
        if(openGoat<98)
        {
            stopTimer(openGoatTimer);
            open100GoatDoor();
        }
    }
    if(timerF == simulTimer)
    {
        if(total<simulNum)
        {
            total = simulNum;
        }
        if(simulNum > 0)
        {
            stopTimer(simulTimer);
            simulation(simulNum-1);
        }
        if(simulNum == 0)
        {
            fin = true;
        }
    }
}
//=====================================================================================
//게임시작
SceneID game_init()
{
    scene[1] = createScene("scene1", "scene1.png");
    start = createObject("start.png", scene[1], 900, 250, true);
    sound1 = createSound("Hidden_Agenda.mp3");
    playSound(sound1, true);
    sound2 = createSound("Sherlock.mp3");
    startbgm = createTimer(1);
    startTimer(startbgm);
    return scene[1];
}



//메인
int main()
{
    srand((int)time(NULL));
    setMouseCallback(mouseCallback);
    setTimerCallback(timerCallback);
    //setSoundCallback(soundCallback); //콜백함수 등록

    //게임 인터페이스 처리
    setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
    Trigger = createTimer(0.01f);
    scene4Count = createTimer(0);
    TotalTrigger = createTimer(0.01f);
    scene4Total = createTimer(0);
    openGoatTimer = createTimer(ANIMATION_TIME);
    simulTimer = createTimer(ANIMATION_TIME);
    
    startGame(game_init());
}
