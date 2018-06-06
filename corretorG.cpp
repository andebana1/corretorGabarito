/*
*Autor: Anderson Vieira de Lima
*E-mail: andersonvieira14@gmail.com
*/

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
/*Mateiral usando para base:
    https://docs.opencv.org
    https://github.com/gabri14el/corretor2gabarito/blob/master/artigo.pdf
*/
using namespace cv;
using namespace std;

RNG rng(12345);
int main(int argc, char** argv)
{
    setlocale(LC_ALL, "Portuguese");
    // Loads an image
    Mat src = imread( "prova0.png", IMREAD_COLOR ), dst, gray, grayL, grayC;
    vector<Vec4i> lines, hierarchy, coordenadasL, coordenadasC;
    int menorX = 600, menorY = 600, maiorX = 0, maiorY = 0, dx, dy, teste, gabCorre[18][5] = {0}, pt = 0, ptl = 0, match = 0;
    int gabarito[18][5] = {{0,0,1,0,0},
                          {0,1,0,0,0},
                          {0,0,0,1,0},
                          {0,0,0,1,0},
                          {0,0,0,1,0},
                          {0,0,0,1,0},
                          {0,1,0,0,0},
                          {0,0,0,1,0},
                          {0,0,0,1,0},
                          {0,0,0,0,1},
                          {1,0,0,0,0},
                          {1,0,0,0,0},
                          {0,1,0,0,0},
                          {0,0,0,1,0},
                          {0,0,0,0,1},
                          {0,0,1,0,0},
                          {1,0,0,0,0},
                          {0,0,0,0,1}};
    vector<vector<Point> > contours;
    // Check if image is loaded fine
    if(src.empty()){
        printf(" Error opening image\n");
        return -1;
    }

    cvtColor(src, gray, COLOR_BGR2GRAY);
    medianBlur(gray, gray, 7);
    grayC = gray.clone();
    threshold(gray, gray,200, 255,THRESH_BINARY_INV);//200


    //Realizando o pré-processsamento, assim apenas os elementos que queremos permanecerão na imagem
    Mat elem1 = getStructuringElement(MORPH_RECT, Size(9,9));//9x9

    morphologyEx(gray, gray, MORPH_ERODE, elem1, Point(-1,-1));
    //resultados

    grayC = gray.clone();
    //Detectando e desenhando as linhas na imagem sgmentada
    Canny(src, dst, 50, 200, 3);
    cvtColor(dst, grayL, COLOR_GRAY2BGR);

    HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        //line( grayL, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
        if(menorX > l[0])
            menorX = l[0];
        if(maiorX < l[2])
            maiorX = l[2];
        if(menorY > l[1])
            menorY = l[1];
        if(maiorY < l[3])
            maiorY = l[3];
    }

    dx = maiorX - menorX;
    dy = maiorY - menorY;

    teste = static_cast<int>(dy/19);

    for (int i = menorY + teste; i < maiorY; i = i + teste) {
        line( grayL, Point(menorX, i), Point(menorX + dx, i), Scalar(0,255,0), 3, CV_AA);
        coordenadasL.push_back(Vec4i(menorX, i, menorX+dx, i));
    }

    //Desenhando as colunas


    teste = static_cast<int>(dx/6);

    for (int i = menorX + teste; i < maiorX + teste; i = i + teste) {
        line(grayL, Point(i, menorY), Point(i, menorY + dy), Scalar(255,0,0), 3, CV_AA);
        coordenadasC.push_back(Vec4i(i, menorY, i, menorY+dy));
    }

    //detectando respostas

    findContours( gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    Mat drawing = Mat::zeros( gray.size(), CV_8UC3 );

    if(contours.size() == 0){
        printf("\n\nProva em Branco: Nota 0!\n");
        return -1;
    }

    //encontrando os centros de massa
    vector<Moments> mu(contours.size());
    for (int i = 0; i < contours.size(); i++){
        mu[i] = moments( contours[i], false );
    }
    //centro de massa : vetor mc
    vector<Point2f> mc( contours.size() );
    for( int i = 0; i < contours.size(); i++ ){
        mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
    }

    for( int i = 0; i< contours.size(); i++ ){
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
       circle( drawing, mc[i], 4, color, -1, 8, 0 );
     }

    //corrigindo prova

    for (int i = 0; i < coordenadasL.size(); i++) {
        Vec4i linha1= coordenadasL[i],linha2 = coordenadasL[i+1];
        for (int j = 0; j < coordenadasC.size(); j++){
            Vec4i coluna1 = coordenadasC[j], coluna2 = coordenadasC[j+1];
            for(int z = contours.size() - 1; z >= 0; z--){
                Point p = mc[z];
                if((p.x >= coluna1[0] && p.x <= coluna2[0]) && (p.y >= linha1[1] && p.y <= linha2[1])){
                    gabCorre[i][j] = 1;
                }
            }
        }
    }

    for ( int i = 0 ; i < 18; i++){
        printf("Questão %d : ", i+1);
        for ( int j = 0; j < 5; j++){
            if(gabCorre[i][j] == 1){
                ptl += 1;
                if(gabarito[i][j] == gabCorre[i][j]){
                    match = 1;
                }
            }
        }
        if(ptl >= 2){
            printf(" Anulada - muitas questões na mesma linha");
        }else if(ptl == 1){
            if(match == 1){
                pt += 10;
                printf(" Questão Correta!");
            }else{
                printf(" Questão incorreta!");
            }
        }else if(ptl == 0){
            printf(" Questão em branco.");
        }
        ptl = 0;
        match = 0;
        printf("\n");
    }

    printf("\n Você fez %d pontos!\n\n", pt);

    /*imshow("Imagem do gabarito", src);
    imshow("separando circulos", gray);
    imshow("detected lines", grayL);
    imshow("Contornos/circulos detectados", drawing);*/

    return 0;
}
