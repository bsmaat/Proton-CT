% params -> step (size of pixel), rho1, rho2 are limits of bp matrix
function [I, dI] = bpFilterSq(bp, params, N)

rho1 = params.rho1;
rho2 = params.rho2;
tau = params.step;

length = tau * size(bp,1);
 
% define variables for x and y positions to create a mesh of r (matrix)
x1 = (rho1:tau:rho2); 
y1 = (rho1:tau:rho2);
[yyy, xxx] = ndgrid(-y1, x1);
r = (xxx.^2 + yyy.^2).^(1/2);

% phi defined as in paper
phi = @(x) ( (pi .* x/2) .* ( besselj(1, x) .* stvh(0,x) - besselj(0,x) .* stvh(1,x)   ) );

indx = find(r==0);
r(indx) = 1; % do this to avoid nan issues as we wil be dividing by r

% computation of k, as in paper
first = (1./(4*pi^2*r.^3));
second = ( (pi*r/tau).^2 .* besselj(1,pi.*r/tau) ...
    - phi(pi.*r/tau) );
k = first.*second;
k(indx) = pi/(12*tau^3);

% perform the FFT and filtering
fftProj = fft2(bp,N,N);
kfft = tau^2 * abs(fft2(k,N,N));
filtered = fftProj .* kfft;
ifftProj = real(ifft2(filtered));

I = ifftProj(1:size(bp,1), 1:size(bp,2));
%I1 = I1(2:size(bp,1)-1,2:size(bp,2)-1); % -1 because of nature of FFT

dI = 0;

%return;
%%%%%%%%%%%%%%% the remainder is the correction.... %%%%%%%%%%%



rho1 = 2*rho1;
rho2 = 2*rho2;
x1 = (rho1:tau:rho2);
y1 = (rho1:tau:rho2);
[yyy, xxx] = ndgrid(-y1, x1);
r = (xxx.^2 + yyy.^2).^(1/2);
indx = find(r==0);
k = (1./(4*pi^2*r.^3)) .* ( (pi*r/tau).^2 .* besselj(1,pi.*r/tau) ...
    - phi(pi.*r/tau) );

k(indx) = pi/(12*tau^3);

kpOut = k./r;


kpOut(ceil(size(kpOut,1)/2)-ceil(size(bp,1)/2)+1:ceil(size(kpOut,1)/2)+ceil(size(bp,1))/2, ...
    ceil(size(kpOut,1)/2)-ceil(size(bp,1))/2+1:ceil(size(kpOut,1)/2)+ceil(size(bp,1)/2)) = 0;

figure; imagesc(kpOut);
dI = (tau^2 * sum(kpOut(:)));

return;




end
