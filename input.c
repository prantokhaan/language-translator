#include<stdio.h>

void sum(int a, int b){
    int c = a+b;
    printf("Sum is %d", c);
}

int main(){
    char string[100];
    int a = 10;
    int b, c, d;
    for(int i=0; i<10; i++){
        a+=i;
    }

    int ff = b*c+d;
    ff--;
    ff = (b+c)*d;
    /*
    this is a comment
    */
    if(a>20){
        printf("a is greater than 20");
    }
    else{
        printf("a is less than 20");
    }

    while(a<b){
        a++;
    }


    switch(a){
        case 1:
            printf("a is 1");
            break;
        case 2:
            printf("a is 2");
            break;
        default:
            printf("a is neither 1 nor 2");
            break;
    }

    sum(a+5, b);

    double abcd;
}