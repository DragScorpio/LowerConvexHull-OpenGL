//  Finding Upper Envelope Lines
//  Created by Xiangzhen Sun on 11/7/17.
//  Copyright © 2017 Xiangzhen Sun. All rights reserved.
//  Deployment target: MacOS 10.8
/*
 Lemma: Let P be a set of points in the plane. The counterclockwise
        order of the points along the upper (lower) convex hull of P , is
        equal to the left to right order of the sequence of lines on the lower
        (upper) envelope of the dual *P.
 */

/* click ENTER to generate upper envelope of n lines
   click ESC to quit the program
 */

#include <iostream>
#include <vector>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

// window size:
#define WIDTH 1000
#define HEIGHT 750
using namespace std;

int mouseClick = 0;  // record the 1st/2nd mouse-clicking
// detect key operation:
int winId;
void keyOperations ( unsigned char key, int x, int y );

/*
 lines (segments) can be determined by 2 points in a 2-D plane
 line expression: y = slope * x - intercept
 */
typedef struct LINE_SEGMENT {
    int x1;
    int y1;
    int x2;
    int y2;
    float slope;
    float intercept;
} LineSegment;

vector<LineSegment> segs;  // vector 'segs' stores line segments

// this function helps draw a line indicated by 'seg'
void drawLine( LineSegment seg ) {
    pair<float, float> end1, end2;  // 2 ends of a line that extends out of "screen"
    glLineWidth( 0.2 );
    glColor3f( 1.0, 0.5, 0.0 );
    glBegin( GL_LINES );
    
    end1.first = 0;
    end1.second = 0 - seg.intercept;
    end2.first = WIDTH;
    end2.second = seg.slope * WIDTH - seg.intercept;
    glVertex2f(end1.first, end1.second);
    glVertex2f(end2.first, end2.second);
    
    glEnd();
    glutSwapBuffers();
}

void customizeLine( pair<float, float> leftEnd, pair<float, float> rightEnd, GLfloat thickness ) {
    glColor3f( 0.0, 0.1, 1.0 );
    glLineWidth( thickness );
    glBegin( GL_LINES );
    glVertex2f(leftEnd.first, leftEnd.second);
    glVertex2f(rightEnd.first, rightEnd.second);
    glEnd();
    
    glutSwapBuffers();
}

// define mouse action
void mouseAction( int button, int state, int x, int y ) {
    LINE_SEGMENT line;
    if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
        switch( mouseClick ) {
            case 0:  // the first click of mouse
                LineSegment newSeg;  // make a new line segment
                newSeg.x1 = newSeg.x2 = x;
                newSeg.y1 = newSeg.y2 = HEIGHT - y;
                segs.push_back( newSeg );
                mouseClick = 1;
                break;
            case 1:  // on the second click of mouse, draw a line
                segs.back().x2 = x;
                segs.back().y2 = HEIGHT - y;
                
                // prevent verticle line by shifting
                if( segs.back().x2 == segs.back().x1 )
                    segs.back().x2 += 3;
                
                line = segs.back();
                segs.back().slope = ( line.y2 - line.y1 ) * 1.0 / ( line.x2 - line.x1 );
                segs.back().intercept = segs.back().slope * line.x1 - line.y1;
                
                /* test output
                cout << "Input Line: (x1,y1),(x2,y2),(a,b)" << endl;
                cout << "(" << segs.back().x1 << ',' << segs.back().y1 << ")" << ',' << "(" << segs.back().x2 << ','
                << segs.back().y2 << ")" << ',' << "(" << segs.back().slope << ',' << segs.back().intercept << ")" << endl;
                */
                
                mouseClick = 0;
                drawLine( segs.back() );
                break;
        }
    }
}

void display() {
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glColor3f( 0.0f, 0.0f, 0.0f );
    glClear(GL_COLOR_BUFFER_BIT);  //Clear the colour buffer (more buffers later on)
    glFlush();
}

void init( void ) {
    glViewport( 0, 0, WIDTH, HEIGHT );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // local coordinate: x: 0 -> WIDTH  y: 0 -> HEIGHT
    gluOrtho2D(0.0f, (GLdouble)WIDTH, 0.0f, (GLdouble)HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

// Sort criterion: points (duals) are sorted with respect to their x-coordinate.
//                 If two points have the same x-coordinate then we compare
//                 their y-coordinates
bool sortPoints( const LineSegment &lhs, const LineSegment &rhs ) {
    return ( lhs.slope < rhs.slope ) || ( lhs.slope == rhs.slope && lhs.intercept < rhs.intercept );
}

// Check if three points make a right turn using cross product
bool rightTurn( const LineSegment &P1, const LineSegment &P2, const LineSegment &P3 ) {
    return ( (P3.slope - P1.slope) * (P2.intercept - P1.intercept) - (P3.intercept - P1.intercept) * (P2.slope - P1.slope) ) > 0;
}

// find lower convex hull of input lines' duals
// Algorithm: Andrew's monotone chain convex hull algorithm
vector<LineSegment> lowerConvexHull( vector<LineSegment> lines ) {
    int n_points = static_cast<int>( lines.size() );
    vector<LineSegment> lowerCH;
    
    sort( lines.begin(), lines.end(), sortPoints );
    /* test output
    cout << "Sorted Dual-Points" << endl;
    for( int i = 0; i != n_points; i++ )
        cout << "(" << lines[i].slope << " , " << lines[i].intercept << ")" << endl;
    */
    
    // computing the lower convex hull
    lowerCH.push_back( lines[n_points-1] );
    lowerCH.push_back( lines[n_points-2] );
    
    for( int i = 2; i < n_points; i++ ) {
        while( lowerCH.size() > 1 and ( !rightTurn( lowerCH[lowerCH.size()-2], lowerCH[lowerCH.size()-1], lines[n_points-i-1] ) ) )
            lowerCH.pop_back();
        
        lowerCH.push_back( lines[n_points-i-1] );
    }
    
    /* test output
    cout << "Lower Covex Hull" << endl;
    for( int i = 0; i < lowerCH.size(); i++ )
        cout << "(" << lowerCH[i].slope << " , " << lowerCH[i].intercept << ")" << endl;
    */
    
    return lowerCH;
}

// calculate the intersection of two lines
// rule:
// let (x, y) be the intersection, there must be:
// y = Ax - B && y = Cx - D ==> x = (B-D)/(A-C), y = Ax - B
pair<float, float> intersection( const LineSegment &l1, const LineSegment &l2 ) {
    pair<float, float> point;
    point.first = ( l1.intercept - l2.intercept ) / ( l1.slope - l2.slope );
    point.second = l1.slope * point.first - l1.intercept;
    return point;
}

// If the 'g' key has been pressed, calculate and display upper envelope
void keyOperations ( unsigned char key, int x, int y ) {
    vector<LineSegment> lch;  // stores lines in lower convex hull
    pair<float, float> rightEnd, leftEnd;
    switch( key ) {
        case 13:  // hit "ENTER" to generate upper evelope
            lch = lowerConvexHull( segs );
            // start point of drawing: to the right-most of screen
            rightEnd.first = (GLdouble)WIDTH;
            rightEnd.second = lch[0].slope * rightEnd.first - lch[0].intercept;
            
            for( int i = 0; i < lch.size() - 1; i++ ) {
                leftEnd = intersection( lch[i], lch[i+1] );
                customizeLine( leftEnd, rightEnd, 5.0 );
                rightEnd = leftEnd;
            }
            
            // end point of drawing: to the left-most of screen
            leftEnd.first = 0.0;
            leftEnd.second = lch.back().slope * leftEnd.first - lch.back().intercept;
            customizeLine( leftEnd, rightEnd, 5.0 );
            break;
        case 27:  // hit ESC to quit the program
            glutDestroyWindow( winId );
            exit(0);
            break;
    }
}

int main( int argc, char* argv[] ) {
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_SINGLE|GLUT_RGB );
    glutInitWindowSize( WIDTH, HEIGHT );
    glutInitWindowPosition( 200, 200 );
    winId = glutCreateWindow( "upper envelope of segments" );
    glutDisplayFunc( display );
    glutMouseFunc( mouseAction );
    glutKeyboardFunc( keyOperations );  // Tell GLUT to use the method "keyPressed" for key presses
    init();
    glutMainLoop();
    
    return 0;
}
