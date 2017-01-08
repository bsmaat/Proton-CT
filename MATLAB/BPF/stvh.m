function [ sh0 ] = stvh(n, x)
%UNTITLED13 Summary of this function goes here
%   Detailed explanation goes here

if (n==0)
    for i=1:size(x,1)
        for j=1:size(x,2)
            sh0(i,j) = stvh0(x(i,j));
        end
    end
end

if (n==1)
    for i=1:size(x,1)
        for j=1:size(x,2)
            sh0(i,j) = stvh1(x(i,j));
        end
    end
end


