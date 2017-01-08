% NB: no need for stepSize i think?
% p are the polynomial coefficients used to fit 1/(beta^2 p^2) to 5th degree polynomial. find these separately or take from literature or from billy's paper 

function [ ymlp ] = mlp2( y0, y2, u0,u1,u2,stepSize, p )

ymlp = zeros(size(u1,2),2);
ymlp(1,:) = y0';
ymlp(end,:) = y2';

X0 = 36.1; %cm
X0 = X0*10; % mm
E0 = 13.6; % MeV/cm
E0 = E0/10; %MeV/mm

% a0 = p(6);
% a1 = p(5);
% a2 = p(4);
% a3 = p(3);
% a4 = p(2);
% a5 = p(1);

% changing scaling...
a0 = p(6);
a1 = p(5)/10;
a2 = p(4)/10^2;
a3 = p(3)/10^3;
a4 = p(2)/10^4;
a5 = p(1)/10^5;

% this one is good
fst = @(u1, u0) (E0^2 * (1 + 0.038*log((u1 - u0)/X0)).^2/X0) .* ...
    ( (1./1).*(u1.^2.*a0                 ).*(u1-u0) ...
    + (1./2).*(u1.^2.*a1 - 2.*u1.*a0     ).*(u1.^2-u0.^2) ...
    + (1./3).*(u1.^2.*a2 - 2.*u1.*a1 + a0).*(u1.^3-u0.^3) ...
    + (1./4).*(u1.^2.*a3 - 2.*u1.*a2 + a1).*(u1.^4-u0.^4) ...
    + (1./5).*(u1.^2.*a4 - 2.*u1.*a3 + a2).*(u1.^5-u0.^5) ...
    + (1./6).*(u1.^2.*a5 - 2.*u1.*a4 + a3).*(u1.^6-u0.^6) ...
    + (1./7).*(          - 2.*u1.*a5 + a4).*(u1.^7-u0.^7) ...
    + (1./8).*(          	         + a5).*(u1.^8-u0.^8) ...
    );

% this one is good!
fsth = @(u1,u0) (E0^2 * ( 1 + 0.038*log((u1 - u0)/X0)).^2/X0) .* ...
    ( a0.*(u1-u0) + a1.*(u1.^2-u0.^2)./2 + a2.*(u1.^3-u0.^3)./3 ...
    + a3.*(u1.^4-u0.^4)./4 + a4.*(u1.^5-u0.^5)./5 + a5.*(u1.^6-u0.^6)./6 );

% this is good!
fstth = @(u1,u0) (E0^2 * (1 + 0.038*log((u1 - u0)/X0)).^2/X0) .* ...
    ( (1./1).*u1.*a0.*(u1-u0) ...
    + (1./2).*(u1.*a1 - a0).*(u1.^2-u0.^2) ...
    + (1./3).*(u1.*a2 - a1).*(u1.^3-u0.^3) ...
    + (1./4).*(u1.*a3 - a2).*(u1.^4-u0.^4) ...
    + (1./5).*(u1.*a4 - a3).*(u1.^5-u0.^5) ...
    + (1./6).*(u1.*a5 - a4).*(u1.^6-u0.^6) ...
    + (1./7).*(       - a5).*(u1.^7-u0.^7) ...
    );

for i = 2:size(u1,2)-1

    st1(i) = fst(u1(i), u0);
    sth1(i) = fsth(u1(i), u0);
    stth1(i) = fstth(u1(i), u0);
    
    st2(i) = fst(u2, u1(i));
    sth2(i) = fsth(u2, u1(i));
    stth2(i) = fstth(u2, u1(i));
    
    SIG1 = [st1(i) stth1(i); stth1(i) sth1(i)];
    SIG2 = [st2(i) stth2(i); stth2(i) sth2(i)];
    
    R0(:,:) = [1 u1(i) - u0; 0 1];
    R1(:,:) =  [1 u2 - u1(i); 0 1]; 
    transposeR1 = transpose(R1);
    
    %firstTerm = inv(SIG1) + transposeR1 * (SIG2 \ R1);
    firstTerm = inv(SIG1) + transposeR1 * (SIG2 \ R1);
    secondTerm = (SIG1\R0)*y0;
    thirdTerm = transposeR1*(SIG2\y2); 
    ymlp(i,:) = firstTerm \ (secondTerm + thirdTerm);
    %ymlp(i) = (1/(firstTerm(1,1)*firstTerm(2,2) - firstTerm(1,2)*firstTerm(2,1))) * dot(firstTerm(1,:),secondTerm+thirdTerm);
    %mm(i) = (1/(firstTerm(1,1)*firstTerm(2,2) - firstTerm(1,2)*firstTerm(2,1)));
    %mm(i) = (1/(firstTerm(1,1)*firstTerm(2,2)));
    %tt(i) = (1/(firstTerm(2,1)));
    %tt = transposeR1 * (SIG2 \ R1);
    %mm(i) = tt(1,1);
end

% no obvious discontinuities
% figure; plot(u1(2:end-1),st1(2:end))
% figure; plot(u1(2:end-1),sth1(2:end))
% figure; plot(u1(2:end-1),stth1(2:end));
% figure; plot(u1(2:end-1),st2(2:end))
% figure; plot(u1(2:end-1),sth2(2:end))
% figure; plot(u1(2:end-1),stth2(2:end))
%figure;
 %plot(u1(2:end),mm, 'r')
% hold on
% plot(u1(2:end),tt, 'b')
end

