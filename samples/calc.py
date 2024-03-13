def lagrange(r,g0,g1,g2):
    print((g0*(r-1)*(r-2)/2))
    print(-g1*r*(r-2))
    print((g2*r*(r-1)/2))
    return (g0*(r-1)*(r-2)/2 - g1*r*(r-2)+g2*r*(r-1)/2)%524287



if __name__ == '__main__':
    # print(lagrange(70, 6,6,2))
    print(lagrange(7000, 126302, 144376, 293598))