% See paper "Two Finite Inverse Hilbert Transform Formulae for Region-of-Interest
% Tomography" for definition of q, r, R, and hilber transform formula

q = 120.0; % boundary of ROI (shepp logan edge is more than 100 mm!)
r = q + 2; 
R = r + 2; % needs to be an integer, in this case...
ds = 1;
dt = 1;
s = -124:ds:124; % min and max value in bp matrix, referred to as bpSum

tolerance = 0.000001;

dx = 0.000000001;
img = [];
Fbefore = [];
Fafter = [];
F = [];
parfor row = 1:249 % 1 : numberOfRows in bp matrix
    row
    F = bpSum(row,:)/(2*pi); % the weighted backprojection...
    
    % make the k
    k = [];

    f = [];
    Fbefore = [];
    Fafter = [];
    counter = 0;
    for sp = -R:ds:R
        counter = counter + 1;
        if abs(sp) < r
            k(counter) = -sqrt(R^2 - sp^2)/pi + sqrt(r^2 - sp^2)/pi;
        elseif abs(sp) <= R
            k(counter) = -sqrt(R^2 - sp^2)/pi;
        end
        
    end
    
    % need to find elements in F we are concerned with
    [c indx1] = min(abs(s - (-R)));
    [c indx2] = min(abs(s - R));
    
    F = F(indx1:indx2);
    F = F.*k;
    counter2 = 1;
    
    for t = -q:dt:q
        firstTerm = 1./(sqrt(R^2 - t^2) - sqrt(r^2 - t^2));
        secondTerm = 0;
        %divisor = ones(1,size(F,2));
        divisor = t - s(indx1:indx2);
        Ft = F./divisor;
        [c index] = min(abs(divisor));
        if divisor(index) == 0
            indx11 = index - 1;
            indx22 = index + 1;
            Fbefore = interp1([-ds 0], [F(index-1) F(index)], -dx);
            Fbefore = Fbefore/dx;
            
            Fafter = interp1([0 ds], [F(index) F(index+1)], dx);
            Fafter = Fafter/-dx;
        else
            disp('should never be here')
            if divisor(index) > 0
                indx22 = index+1;
                indx11 = index;
            else
                indx11 = index-1;
                indx22 = index;
            end
        end

        int1 = trapz(Ft(1:indx11))* ds + trapz([Ft(indx11) Fbefore]) * (ds - dx);
        int2 = trapz(Ft(indx22:end))* ds + trapz([Fafter Ft(indx22)]) * (ds - dx);
        secondTerm = int1 + int2;
        f(counter2) = firstTerm * secondTerm;
        counter2 = counter2 + 1;
%         plot([Ft(1:indx11) Fbefore Ft(index) Fafter Ft(indx22:end)]);
%         ylim([-5 5]);
%         drawnow;
%         pause(0.2);
    end
    
    
    img(row,:) = f;
end
