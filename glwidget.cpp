#include "glwidget.h"

GLfloat ** makeArrayF(GLfloat *from, uint vec_num, uint vec_size)
{

    GLfloat ** to;
    to = (GLfloat**)qMalloc(vec_num*sizeof(GLfloat*));
    for(int i=0; i<vec_num; i++)
        to[i] = (GLfloat*)qMalloc(vec_size*sizeof(GLfloat));

    uint count =0;
    for(int i = 0; i < vec_num; ++i)
        for(int j = 0; j < vec_size; ++j)
        {
            to[i][j] = from[count++];
        }
    return to;
}

GLubyte ** makeArrayUB(GLubyte *from, uint vec_num, uint vec_size)
{
    GLubyte ** to;
    to = (GLubyte**)qMalloc(vec_num*sizeof(GLubyte*));
    for(int i=0; i<vec_num; i++)
        to[i] = (GLubyte*)qMalloc(vec_size*sizeof(GLubyte));

    uint count =0;
    for(int i = 0; i < vec_num; ++i)
        for(int j = 0; j < vec_size; ++j)
        {
            to[i][j] = from[count++];
        }
    return to;
}

void normalizeAngle(GLdouble *angle)
{
    if (qAbs(*angle > 360.0)) *angle = 0;

}


Level::Level (int diff,  QObject *parent):QObject(parent),
    difficulty(diff),
    isActive(true),
    startPoint(QPoint(1,1)),
    transparency(0.0),
    zPos(0.0)
{
    generateMap(diff);
    ball = new Ball(QVector3D(-PLANE_HS+startPoint.x()*2*HS+HS, -PLANE_HS+startPoint.y()*2*HS+HS, 0));
}

Level::Level (int diff, QPoint _startPt, QObject * parent): QObject(parent),
    difficulty(diff),
    isActive(false),
    transparency(0.5),
    zPos(-2*SIDE_LENGTH),
    ball(NULL)
{
    startPoint = _startPt;
    generateMap(diff);
    qDebug()<<startPoint;
    qDebug()<<"Constructor of "<<this<<" finished successfuly";
}

void Level::createBall()
{
    ball = new Ball(QVector3D(-PLANE_HS+startPoint.x()*2*HS+HS, -PLANE_HS+startPoint.y()*2*HS+HS, 0));
}

void Level::drawCube (GLubyte x, GLubyte y)
{
    static GLfloat vertices[] = {HS,HS,HS,  -HS,HS,HS,  -HS,-HS,HS,  HS,-HS,HS,
                                 HS,HS,HS,  HS,-HS,HS,  HS,-HS,-HS,  HS,HS,-HS,
                                 HS,HS,HS,  HS,HS,-HS,  -HS,HS,-HS,  -HS,HS,HS,
                                 -HS,HS,HS,  -HS,HS,-HS,  -HS,-HS,-HS,  -HS,-HS,HS,
                                 -HS,-HS,-HS,  HS,-HS,-HS,  HS,-HS,HS,  -HS,-HS,HS,
                                 HS,-HS,-HS,  -HS,-HS,-HS,  -HS,HS,-HS,  HS,HS,-HS};

    static GLfloat normals[] = {0,0,1,  0,0,1,  0,0,1,  0,0,1,
                                1,0,0,  1,0,0,  1,0,0, 1,0,0,
                                0,1,0,  0,1,0,  0,1,0, 0,1,0,
                                -1,0,0,  -1,0,0, -1,0,0,  -1,0,0,
                                0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0,
                                0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1};

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, &vertices);
    glNormalPointer(GL_FLOAT, 0, &normals);

    glPushMatrix();
    glTranslatef(-PLANE_HS+HS+x*2*HS, -PLANE_HS+HS+y*2*HS, zPos+HS);
    glDrawArrays(GL_QUADS, 0, 24);
    glPopMatrix();

    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void Level::draw()
{
    static GLfloat vertices[4][3] = {   PLANE_HS, PLANE_HS, 0,
                                        -PLANE_HS, PLANE_HS, 0,
                                        -PLANE_HS, -PLANE_HS, 0,
                                        PLANE_HS, -PLANE_HS, 0};
    static GLfloat normals[4][3] = {    0.0, 0.0, 1.0,
                                        0.0, 0.0, 1.0,
                                        0.0, 0.0, 1.0,
                                        0.0, 0.0, 1.0
                                   };
    static GLfloat clr_white[] = {1.0, 1.0, 1.0, 1.0};
    static GLfloat clr_black[] = {0.0, 0.0, 0.0, 1.0};
    static GLfloat clr_wall[] = {0.3, 0.3, 0.4, 1.0};
    static GLfloat clr_plane[] = {0.9, 0.9, 0.9, 1.0};

    clr_white[3] = 1.0 - transparency;
    clr_black[3] = 1.0 - transparency;
    clr_wall[3] = 1.0 - transparency;
    clr_plane[3] = 1.0 - transparency;


    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, clr_plane);
    glMaterialfv(GL_FRONT, GL_SPECULAR, clr_white);
    glMaterialf(GL_FRONT, GL_SHININESS, 100.0);

    glVertexPointer(3, GL_FLOAT, 0, &vertices);
    glNormalPointer(GL_FLOAT, 0, &normals);

    glPushMatrix();
    glTranslatef(0,0,zPos);
    glDrawArrays(GL_QUADS, 0, 4);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, clr_wall);
    for(GLubyte x = 0; x < SIDE_LENGTH; x++)
        for(GLubyte y = 0; y < SIDE_LENGTH; y++)
        {
            if(map[x][y] == 1)
                drawCube(x,y);
            else
                if (map[x][y] == 2)
                {
                    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, clr_black);
                    glPushMatrix();
                    glTranslatef(-PLANE_HS+HS+x*2*HS, -PLANE_HS+HS+y*2*HS, 0);

                    glBegin(GL_QUADS);
                    glVertex3f(-HS,-HS,0.01);
                    glVertex3f(HS,-HS,0.01);
                    glVertex3f(HS,HS,0.01);
                    glVertex3f(-HS,HS,0.01);
                    glEnd();
                    glPopMatrix();
                    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, clr_wall);
                }
        }
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, clr_white);

    if(ball != NULL)
        ball->draw();
}

void Level::inclineChanged(double ix, double iy)
{
    ball->inclineChanged(ix, iy);
}

void Level::update()
{
    qDebug()<<"updating "<<this;
    if(zPos < 0 && isActive)
    {
        zPos += 1.0;
        if (transparency > 0) transparency -= 0.02;
    }
    if(isActive && ball == NULL)
        ball = new Ball(QVector3D(-PLANE_HS+startPoint.x()*2*HS+HS, -PLANE_HS+startPoint.y()*2*HS+HS, 0));
    if(ball != NULL)
    {
        checkForCollisions();
        ball->updateCalculations();
        if(isEnd())
        {
            getDestroyed();
        }
    }
    qDebug()<<"update successful";
}

struct Plane
{
    QVector3D point, normal;
    Plane(QVector3D _normal, QVector3D _point):point(_point), normal(_normal){}
};

void Level::checkForCollisions()
{
    int x = floor((ball->getCenter().x()+PLANE_HS)/(2*HS));
    int y = floor((ball->getCenter().y()+PLANE_HS)/(2*HS));

    QList<Plane> faces;

    if (map[x+1][y] == 1)
        faces.append(Plane(QVector3D(-1,0,0), QVector3D(-PLANE_HS+2*HS*(x+1), 0, 0)));
    if (map[x-1][y] == 1)
        faces.append(Plane(QVector3D(1,0,0), QVector3D(-PLANE_HS+2*HS*(x), 0, 0)));
    if (map[x][y+1] == 1)
        faces.append(Plane(QVector3D(0,-1,0), QVector3D(0, -PLANE_HS+2*HS*(y+1), 0)));
    if (map[x][y-1] == 1)
        faces.append(Plane(QVector3D(0,1,0), QVector3D(0, -PLANE_HS+2*HS*(y), 0)));

    foreach (Plane face, faces)
    {
        if(qAbs(ball->getCenter().distanceToPlane(face.point, face.normal)) <= BALL_R+0.01)
        {
            ball->collisionFrom(face.normal);
        }
        else
        {
            ball->endCollision(face.normal);
        }
    }
}

bool Level::isEnd()
{
    qDebug()<<"checking for end";
    int x = floor((ball->getCenter().x()+PLANE_HS)/(2*HS));
    int y = floor((ball->getCenter().y()+PLANE_HS)/(2*HS));

    if(endPoint != NULL)
        if(x == endPoint->x() && y == endPoint->y())
        {
            qDebug()<<"check successful";
            return true;
        }
        else
        {
            qDebug()<<"check successful";
            return false;
        }
    else qDebug()<<"end pt is NULL";
}

QPoint Level::getEndPt()const
{
    QPoint pt = *endPoint;
    qDebug()<<"getEndPt:"<<pt;
    return pt;
}

void getDir(int &dx, int &dy)
{
    do
    {
        dx = rand()%3-1;
        dy = rand()%3-1;
    }
    while(abs(dx) == abs(dy));
}

uint recStep(GLubyte map[SIDE_LENGTH][SIDE_LENGTH], int _diff, int x, int y, int _step, QPoint *exit)
{
    uint step = 0;
    const uint fullfill = (uint)(SIDE_LENGTH*SIDE_LENGTH)/(30-_diff);
    uint laststep = _step, fill = 0;
    const uint attempts = 1000;
    step = _step;
    int dx = 0;
    int dy = 0;
    int cntr = 0;
    do
    {
        getDir(dx, dy);
        cntr++;
    }
    while ((abs(dx) == 1 && map[x+dx][y] == 0  ||
            abs(dy) == 1 && map[x][y+dy] == 0  ||
            abs(dx) == 1 && map[x+dx][y+1] == 0  ||
            abs(dx) == 1 && map[x+dx][y-1] == 0  ||
            abs(dy) == 1 && map[x+1][y+dy] == 0 ||
            abs(dy) == 1 && map[x-1][y+dy] == 0 ||
            x+dx == 0||
            x+dx == SIDE_LENGTH-1||
            y+dy == 0||
            y+dy == SIDE_LENGTH-1
            )&& cntr < attempts);
    if (cntr  >= attempts || step > 400 || fill > fullfill)
    {
        if(exit != NULL)
        {
            exit->rx() = x;
            exit->ry() = y;
        }
        return step;
    }
    else
    {
        x += dx;
        y += dy;
        map[x][y] = 0;
        fill ++;
    }
    laststep = recStep(map,_diff, x, y, ++step, exit);
    if (step < rand()%40)
    {
        recStep(map,_diff, x, y, ++step, NULL);
    }
}

void Level::generateMap(GLubyte difficulty)
{
    for (int i = 0; i < SIDE_LENGTH; ++i)
        for (int j = 0; j < SIDE_LENGTH; ++j)
            map[i][j] = 1;

    map[startPoint.x()][startPoint.y()] = 0;
    srand(QTime::currentTime().msec());
    endPoint = new QPoint();
    recStep(map,difficulty,startPoint.x(),startPoint.y(),0, endPoint);
    map[endPoint->x()][endPoint->y()]=2;
}

void Level::getStarted()
{
    isActive = true;
}

void Level::getDestroyed()
{
    delete ball;
    ball = NULL;
    emit over(getEndPt());
    delete endPoint;
    endPoint = NULL;
}

GLWidget :: GLWidget (QWidget *parent):
    rotxAngle(0),
    rotyAngle(0)
{
    this->setGeometry(20,20, HEIGHT,WIDTH);
    timer = new QTimer;
    timer->setInterval(TIMER_INTERVAL);
    currentLevel = new Level(25);
    nextLevel = new Level(25, currentLevel->getEndPt());

    connect(timer, SIGNAL(timeout()), currentLevel, SLOT(update()));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    connect(this, SIGNAL(inclineChanged(double,double)), currentLevel, SLOT(inclineChanged(double,double)));
    connect(currentLevel, SIGNAL(over(QPoint)), this, SLOT(switchLevel(QPoint)));

    timer->start();
}

void GLWidget :: initializeGL()
{
    static GLfloat colors[10][4] = {
        0.0, 0.0, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0,
        0.1, 0.1, 0.1, 1.0,
        1.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        1.0, 0.6, 0.0, 1.0,
        1.0, 1.0, 0.0, 1.0,
        0.7, 0.7, 1.0, 1.0,
        1.0, 0.7, 1.0, 1.0
    };

    glEnable(GL_MATRIX_MODE);
    glEnable(GL_DEPTH_TEST);
    glDepthRange(-0.5, 500.0);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(70, 1, 0.1, 500.0);

    glViewport(0, 0, WIDTH, HEIGHT);

    GLfloat lightPos[] = {100.0, 100.0, 100.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, colors[c_white]);
    glLightfv(GL_LIGHT0, GL_SPECULAR, colors[c_white]);

    glFrontFace(GL_CCW);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
    glLightModelfv(GL_AMBIENT, colors[c_black]);
    glEnable(GL_LIGHT0);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(5);
    glPointSize(10);
}


void GLWidget:: paintGL()
{
    qDebug()<<"painting";
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    QVector3D cam = QVector3D(1.7*PLANE_HS,1.7*PLANE_HS,3*PLANE_HS);
    QVector3D up =  QVector3D::normal(cam, QVector3D(-cam.x(),cam.y(),0));
    gluLookAt(cam.x(),cam.y(),cam.z() ,0,0,0, up.x(), up.y(), up.z());
    glRotatef(rotxAngle, 1,0,0);
    glRotatef(rotyAngle, 0,1,0);
    qDebug()<<"-drawing"<<currentLevel;
    currentLevel->draw();
    qDebug()<<"-successful";
    qDebug()<<"-drawing"<<nextLevel;
    nextLevel->draw();
    qDebug()<<"successful";
    glPopMatrix();
    qDebug()<<"successful";
}


void GLWidget::switchLevel(QPoint initial)
{
    timer->stop();

    disconnect(timer, SIGNAL(timeout()), currentLevel, SLOT(update()));
    disconnect(this, SIGNAL(inclineChanged(double,double)), currentLevel, SLOT(inclineChanged(double,double)));
    disconnect(currentLevel, SIGNAL(over(QPoint)), this, SLOT(switchLevel(QPoint)));

    delete currentLevel;
    currentLevel = nextLevel;
    currentLevel->getStarted();

    nextLevel = new Level(29, initial);

    connect(timer, SIGNAL(timeout()), currentLevel, SLOT(update()));
    connect(this, SIGNAL(inclineChanged(double,double)), currentLevel, SLOT(inclineChanged(double,double)));
    connect(currentLevel, SIGNAL(over(QPoint)), this, SLOT(switchLevel(QPoint)));

    timer->start();
}

void GLWidget:: mousePressEvent(QMouseEvent *e)
{
    mousePressPosition = QVector3D(e->posF().x(), 0, HEIGHT/2).normalized();
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    mouseReleasePosition = QVector3D(e->posF().x(), 0, HEIGHT/2).normalized();
    qreal angle = 57.9*acos(QVector3D::dotProduct(mousePressPosition, mouseReleasePosition)-0.0001);  // �������� 0.0001 ����� ��������� ������ ������ �������� ����
    if(mousePressPosition.x()<mouseReleasePosition.x())
        camRotzAngle += angle;
    else
        camRotzAngle -= angle;
    normalizeAngle(&camRotzAngle);
    mousePressPosition = mouseReleasePosition;
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e)
{

}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case(Qt::Key_Up):
        if(rotxAngle <= 30)
            rotxAngle += 1;
        emit inclineChanged(rotxAngle, rotyAngle);
        break;
    case(Qt::Key_Down):
        if(rotxAngle >= -30)
            rotxAngle -= 1;
        emit inclineChanged(rotxAngle, rotyAngle);
        break;
    case(Qt::Key_Left):
        if(rotyAngle <= 30)
            rotyAngle += 1;
        emit inclineChanged(rotxAngle, rotyAngle);
        break;
    case(Qt::Key_Right):
        if(rotyAngle >= -30)
            rotyAngle -= 1;
        emit inclineChanged(rotxAngle, rotyAngle);
        break;
    case(Qt::Key_End):
        switchLevel(currentLevel->getEndPt());
    }
}
