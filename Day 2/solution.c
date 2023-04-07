#include <stdio.h>


int SCORETABLE_RPS[3][3] = { // Table of game result [opponent][me] for rock-paper-scissor
                                //X   Y   Z 
                                {1+3,2+6,3+0}, // A
                                {1+0,2+3,3+6}, // B
                                {1+6,2+0,3+3}};// C


int SCORETABLE_LDW[3][3] = { // Table of game result [opponent][me] for lose-draw-win
                        //X   Y   Z 
                        {3+0,1+3,2+6}, // A
                        {1+0,2+3,3+6}, // B
                        {2+0,3+3,1+6}};// C


int (*SCORETABLE)[3] = SCORETABLE_LDW; //{{0,0,0},{0,0,0},{0,0,0}}; // scoreboard


int main(int argc, char *argv[]) {
    FILE *fp;
    char player1[2] = "F"; // character + \0
    char player2[2] = "G"; // so fscanf("%1s") reads 1 non-space, non-newline character
    int score=0;
    int score_opp=0;
    int i=0,j=0;


    double mean_score = 0;
    for(i=0;i<3;i++) for(j=0;j<3;j++) mean_score += SCORETABLE[i][j];
    mean_score/=9;

    if (argc == 1) {
        fp = stdin;
    } else { 
        fprintf(stdout,"Opening %s", argv) ;
        fp = fopen(*argv,"r");
        if (fp==NULL) {
            perror("Error opening file");
        }
    }


    i=0;// count number of turns in t
    while (fscanf(fp," %1s %1s ",player1, player2)!=EOF && i<100000 ){
        score += SCORETABLE[*player1-'A'][*player2-'X'];
        score_opp += SCORETABLE[*player2-'X'][*player1-'A'];
        if (i%100==0 && i<2500){
            fprintf(stdout,"%c %c : my score: %d, my opponent_score: %d\n",*player1,*player2, SCORETABLE[*player1-'A'][*player2-'X'],SCORETABLE[*player2-'X'][*player1-'A']);
        }
        i++;
    }

    fclose(fp);
    fprintf(stdout,"%d turns were played:\n",i);
    fprintf(stdout,"My score: %d\n",score);
    fprintf(stdout,"My opponent* score: %d\n",score_opp);
    fprintf(stdout,"(*Not the score of any actual opponent, compare mean score %.0f)",mean_score*i);

    
    return 0;

}
