
int main()
{
    int a, b, c;
    if (a) {
        a++;
    }
    else{
        a--;
    }
label:
    switch (b)
    {
    case 0:
        c++;
        break;
    case 1:
    {
        c++;
        break;
    }

    
    default:
        goto label;
        break;
    }
    int a1 = 0;
}