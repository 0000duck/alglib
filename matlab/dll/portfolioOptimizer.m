function [cost, target] = portfolioOptimizer(covMatrix, expRet, tCost, weight)
    if ~libisloaded('optimizer')
        loadlibrary('optimizer', 'optimizer.hpp');
    end
    
    [m, n] = size(covMatrix);
    
    assert(m == n, 'Э����������Ϊ����')
    [rm, rn] = size(expRet);
    assert((rm==m && rn==1) || (rm==1 || rn==m), '��������������Э�������ά�Ȳ�ͳһ')
    
    [tm, tn] = size(tCost);
    assert((tm==m && tn==1) || (tm==1 || tn==m), '������������Э�������ά�Ȳ�ͳһ')
    
    [wm, wn] = size(weight);
    assert((wm==m && wn==1) || (wm==1 || wn==m), '��ʼȨ����Э�������ά�Ȳ�ͳһ')
    
    var =  libpointer('doublePtr', covMatrix);
    r = libpointer('doublePtr', expRet);
    t = libpointer('doublePtr', tCost);
    w = libpointer('doublePtr', weight);
    
    target = libpointer('doublePtr', zeros(1, m));
    cost = libpointer('doublePtr', 0.0);
    
    calllib('optimizer', 'portfolioOptimizer', m, var, r, t, w, target, cost);
    cost = double(cost.Value);
    target = double(target.Value);
end

