function xp = motor(t,x)
    Gear = 65.5;
    Ka = (((1.63/1000)*60)/(2*pi))*Gear;
    La = 1.16*10^-3;
    Ra = 1.93;
    Jm = (2.6*10^-5);
    Jpl = 13.95*10^-3;
    Je = Jm*Gear + Jpl;
    Bm = 1.2*10^-5;
    Bpl = .01;
    Be = Bm*Gear + Bpl;
    Eff = .66;
    KFriction = 0.14136;
    
    u=[1 0];
    
    if(x(2) == 0)
        Tfriction = 0;
    else
        Tfriction = KFriction;
    end
    
    xp=zeros(2,1);
    xp(1)=(1/La)*u(1)-(Ra/La)*x(1)-(Ka/La)*x(2);
    xp(2)=((Ka*Eff)/Je)*x(1)-(1/Je)*u(2)-(Be/Je)*x(2)-Tfriction;