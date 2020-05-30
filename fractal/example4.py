import turtle
import random
import math

turtle.title("turtle example") #제목
turtle.setup(width=1000, height=800) #창 크기
screen=turtle.Screen()
turtle.colormode(255) #256개 색상펜

def drawTree(x, y, angle, length, order, base):
    x_alpha = x + (math.cos(math.radians(angle)) * length)
    y_alpha = y + (math.sin(math.radians(angle)) * length)
    print(x_alpha, y_alpha)
    base.goto(x_alpha, y_alpha)
    if order > 0:
        drawTree(x_alpha, y_alpha, angle-30, length*0.8, order-1, base)
        base.up()
        base.setpos(x_alpha, y_alpha)
        base.down()
        drawTree(x_alpha, y_alpha, angle+30, length*0.8, order-1, base)
        base.up()
        base.setpos(x_alpha, y_alpha)
        base.down()
        
base=turtle.Turtle()
base.shape("classic") # 거북이 모양
base.left(90)         # 기본 설정에서 turtle이 오른쪽을 향하므로, 다시 위(나무방향)를 향하게
base.up()             # 다음 라인부터 거북이 이동 경로를 화면에서 제외
base.backward(100)    # 길이 100만큼 아래로 이동
base.down()           # 다음 라인부터 거북이 이동 경로를 화면에 포함

drawTree(0, 0, 90, 75, 5, base)    # 트리를 그리기 위해 트리의 길이와 트리를 그릴 수 있는 객체인 base를 함수 drawTree()의 인자값으로 넘겨

screen.exitonclick()  # 화면에 그리는 작업을 완료한 후에도 꺼지지 않도록 지시
