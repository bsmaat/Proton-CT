
xAxis = 0:0.5:4.0; % xAxis - used to fit to err function
x1 = flip(-100.25:0.25:100.25); % x values of image
y1 = -124.875:0.25:124.875; % y values of image

expression = 'y0 + a * erf((x-b)/(sqrt(2)*w0))';
for n = 0:23
    if (n == 0)
        x = 0; y = 0;
    else
        x = (3 + 4 * n) * sin(1.2*n);
        y = (3 + 4 * n) * cos(1.2*n);
    end
    
    
    [c1 indx1] = min(abs(y1-y));
    [c2 indx2] = min(abs(x1-x));
    
    [yyy, xxx] = ndgrid(y1, x1);
    numPx = 18;
    
    [tmpyyy, tmpxxx] = ndgrid(y1((indx1-numPx):(indx1+numPx)), x1((indx2-numPx):(indx2+numPx)));
    
    tmpImg = img((indx1-numPx):(indx1+numPx),(indx2-numPx):(indx2+numPx));
    counter = 1;
    radial_average = [];
    for radiusmm = 0.0:0.5:4.0
        theta = deg2rad(0:1:359);
        Y = y + radiusmm * cos(theta);
        X = x + radiusmm * sin(theta);
        
        sampled_radial_slice = interp2(tmpxxx, tmpyyy, tmpImg, X, Y);
        radial_average(counter) = mean(sampled_radial_slice);
        counter = counter + 1;
    end
    
    radial_average_normalised = (radial_average-min(radial_average))/(max(radial_average) - min(radial_average));
    radial_average_normalised_tmp = flip(radial_average_normalised);
    g = fittype(expression, 'independent','x','dependent','y');
    myfit = fit(xAxis', radial_average_normalised_tmp', g,'StartPoint',[0.5 1.5 0.2 0.5]);
    res(n+1) = myfit.w0;
    %figure; plot(xAxis, radial_average_normalised_tmp);
    %figure; imagesc(tmpImg);
end
