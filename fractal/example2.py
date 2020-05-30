import turtle
import random
# import math   ; unused package

turtle.title("turtle example") #제목
turtle.setup(width=1000, height=800) #창 크기
screen=turtle.Screen()
turtle.colormode(255) #256개 색상펜

def drawTree(length, b):
    if length > 0:             # 함수의 매개변수 값 length가 0보다 클 경우에만 그려주며, 0보다 작으면 해당 함수는 바로 종료됨 
        b.forward(length)      # 가지를 새로 그림, 그리고 나서의 위치와 현재 각도를 편의상 각각 p, d라고 하겠음
        b.right(20)            # 위치 p로부터 오른쪽으로 20도 기울임, d로부터 -20도 기울여진 상태
        drawTree(length-15, b) # 위치 p로부터 오른쪽 가지를 새로 그림, fractal 구조를 그려주기 위해 현재 길이보다 짧은 길이를 인자값으로 넘겨줌 
        b.left(40)             # 오른쪽 가지를 다 그린 후에 위치 p로부터 왼쪽으로 40도 기울임, d로부터 20도 기울여진 상태
        drawTree(length-15, b) # 위치 p로부터 왼쪽 가지를 새로 그림, fractal 구조를 그려주기 위해 현재 길이보다 짧은 길이를 인자값으로 넘겨줌 
        b.right(20)            # 위치 p로부터 오른쪽으로 20도 기울임, d로부터 0도 기울여진 상태(처음 그리기 시작한 각도와 동일함)
        b.up()                 # 위치 p로부터 그리기 전 위치로 되돌아가기 위한 단계, 아래 단계는 굳이 화면에 표현할 필요가 없음
        b.backward(length)     # 위치 p로부터 그리기 전 위치로 돌아감
        b.down()               # 원래 위치로 돌아왔기 때문에, 다시 화면에 표현할 수 있도록 지시
        
base=turtle.Turtle()
base.shape("classic") # 거북이 모양
base.setpos(0,0)      # 시작지점
base.left(90)         # 기본 설정에서 turtle이 오른쪽을 향하므로, 다시 위(나무방향)를 향하게
base.up()             # 다음 라인부터 거북이 이동 경로를 화면에서 제외
base.backward(100)    # 길이 100만큼 아래로 이동
base.down()           # 다음 라인부터 거북이 이동 경로를 화면에 포함

drawTree(75, base)    # 트리를 그리기 위해 트리의 길이와 트리를 그릴 수 있는 객체인 base를 함수 drawTree()의 인자값으로 넘겨

screen.exitonclick()  # 화면에 그리는 작업을 완료한 후에도 꺼지지 않도록 지시
