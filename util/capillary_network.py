#!/usr/bin/env python3

import argparse
import json
from random import choices

import matplotlib.pyplot as plt
import numpy as np
import openpnm as op

"""
To generate this capillary network, we take into account the distributions of pore diameter,
capillary length and connectivity.
"""

# Parse command-line arguments
parser = argparse.ArgumentParser(description='Generates 2D or 3D network from the centerline.')
parser.add_argument('out_folder',
                    type=str,
                    action='store',
                    metavar='OUT_FOLDER',
                    help='Directory containing output files.')
parser.add_argument('--voxel_size',
                    type=float,
                    action='store',
                    required=True,
                    metavar='VOXEL_SIZE',
                    help='Voxel size [m].')
parser.add_argument('--capillary_length',
                    type=float,
                    action='store',
                    required=True,
                    metavar='CAPILLARY_LENGTH',
                    help='Capillary length [m] for regular networkd (cubic, for example).')
parser.add_argument('--epsilon',
                    type=float,
                    action='store',
                    required=True,
                    metavar='EPSILON',
                    help='Precision [%] for porosity matching.')
parser.add_argument('--delta',
                    type=float,
                    action='store',
                    required=True,
                    metavar='DELTA',
                    help='Increment in capillary length [m] to achieve the target porosity.')
parser.add_argument('--sample_size',
                    type=str,
                    action='store',
                    required=True,
                    metavar='LX,LY,LZ',
                    help='Sample sizes [m] along x, y and z (comma-separated values).')
parser.add_argument('--network_type',
                    type=str,
                    action='store',
                    required=True,
                    metavar='NETWORK_TYPE',
                    choices=['cubic', 'gabriel', 'cubic12'],
                    help='Network type: cubic, gabriel, cubic12 (3D only).')
parser.add_argument('--filename',
                    type=str,
                    action='store',
                    required=False,
                    metavar='FILENAME.json',
                    default='centerlines.json',
                    help='Name of the input JSON file.')
arg = parser.parse_args()

# ------------------------------------------------------------------
#
#              INPUT PARAMETERS SECTION
#
# ------------------------------------------------------------------
# Starting the execution
print('Starting the execution...')

# Further parse sample size values
Lx, Ly, Lz = np.array(arg.sample_size.split(',')).astype(float)

# Calculate derived quantities
Nx = int(Lx/arg.capillary_length) + 1
Ny = int(Ly/arg.capillary_length) + 1
Nz = int(Lz/arg.capillary_length) + 1
Lx_ad = Lx/arg.voxel_size
Ly_ad = Ly/arg.voxel_size
Lz_ad = Lz/arg.voxel_size
ds_ad = arg.capillary_length/arg.voxel_size
delta_ad = arg.delta/arg.voxel_size
Npoints = Nx*Ny*Nz

# Reading centerlines.json obtained from micro CT scan rock tomography
with open(arg.out_folder + '/' + arg.filename, mode='r') as file1:
    data = json.load(file1, encoding='utf-8')

# ------------------------------------------------------------------
#
#              ROCK SAMPLE CENTERLINES SECTION
#
# ------------------------------------------------------------------
# Extract node geometry arrays from JSON of rock sample centerlines
nodes = sorted(data['graph']['nodes'], key=lambda node: int(node['id']))
x = np.array([node['metadata']['node_coordinates']['x'] for node in nodes], dtype=float)
y = np.array([node['metadata']['node_coordinates']['y'] for node in nodes], dtype=float)
z = np.array([node['metadata']['node_coordinates']['z'] for node in nodes], dtype=float)

# Extract link geometry arrays from JSON of rock sample centerlines
edges = sorted(data['graph']['edges'], key=lambda edge: int(edge['id']))
sources = np.array([int(edge['source']) for edge in edges])
targets = np.array([int(edge['target']) for edge in edges])
link_length = np.array([edge['metadata']['link_length'] for edge in edges])
link_squared_radius = np.array([edge['metadata']['link_squared_radius'] for edge in edges])

# Calculating the connectivity of each node of rock sample centerlines
print('#############################################################')
print('Some statistics of rock sample centerlines')
print('#############################################################')
print('nodes = ', len(nodes))
print('edges = ', len(edges))
print('#############################################################')
print('Calculating the nodes connectivity of rock sample centerlines')

connectivity = np.zeros(int(len(nodes)/500), dtype=np.int32)
niter = int(len(nodes)/500)
for i in range(0, int(len(nodes)/500)):
    a = 0
    b = 0
    a = np.sum(sources == i)
    b = np.sum(targets == i)
    connectivity[i] = a + b
    perc = ((1.0*i)/niter)*100
    result = i % 1000
    if result == 0:
        print('progress in % = ', perc)

# Obtaining hystograms and density probability distributions of rock sample centerlines
# Capillary diameter density of probability distribution
D = np.sqrt(link_squared_radius) * 2.0 * arg.voxel_size*1e6
low, high = np.floor(D.min()), np.ceil(D.max())
binsD = np.linspace(low, high, int(high - low) + 1)
histD, edgeshistD = np.histogram(D, bins=binsD, density=True)
plt.hist(D, bins=binsD, density=True)
plt.xlabel(r'Capillary diameter [$\mu$m]')
plt.ylabel(r'Probability Density')
plt.grid()
plt.show()
print('Cumulative probability density sum for diameter distribution = ', histD.sum())

# Capillary length density of probability distribution
L = link_length * arg.voxel_size * 1e6
low, high = np.floor(L.min()), np.ceil(L.max())
binsL = np.linspace(low, high, int(high - low) + 1)
histL, edgeshistL = np.histogram(L, bins=binsL, density=True)
plt.hist(L, bins=binsL, density=True)
plt.xlabel(r'Capillary length [$\mu$m]')
plt.ylabel(r'Probability Density')
plt.grid()
plt.show()
print('Cumulative probability density sum for length distribution = ', histL.sum())

# Capillary connectivity density of probability distribution
low, high = np.floor(connectivity.min()), np.ceil(connectivity.max())
bins_connectivity = np.linspace(low, high, int(high - low) + 1)
hist_connectivity, edgeshist_connectivity = np.histogram(connectivity,
                                                         bins=bins_connectivity,
                                                         density=True)
plt.hist(connectivity, bins=bins_connectivity, density=True)
plt.xlabel(r'Capillary connectivity')
plt.ylabel(r'Probability Density')
plt.grid()
plt.show()
print('Cumulative probability density sum for connectivity = ', hist_connectivity.sum())

# ------------------------------------------------------------------
#
#              END OF ROCK SAMPLE CENTERLINES SECTION
#
# ------------------------------------------------------------------
#
# Generating a capillary network

output_file = open(arg.out_folder + '/porosity.txt', mode='w')

output_file.write('###################################################'+'\n')
output_file.write('      Target Porosity: Rock sample centerlines     '+'\n')
output_file.write('###################################################'+'\n')
output_file.write('Target Porosity     Capillary Network Porosity     '+'\n')

porosity_diff = 1.0
kk = 1
while (abs(porosity_diff) > arg.epsilon):
    Nx = int(Lx_ad/ds_ad) + 1
    Ny = int(Ly_ad/ds_ad) + 1
    Nz = int(Lz_ad/ds_ad) + 1
    Npoints = Nx*Ny*Nz
    if arg.network_type == 'cubic':
        if Nz == 0:
            Nz = 1
        print('Nx,Ny,Nz', Nx, Ny, Nz)
        pn = op.network.Cubic(shape=[Nx, Ny, Nz], spacing=[ds_ad, ds_ad, ds_ad])

    if arg.network_type == 'cubic12':
        if Nz > 2:
            pn = op.network.Cubic(shape=[Nx, Ny, Nz],
                                  spacing=[ds_ad, ds_ad, ds_ad],
                                  connectivity=12)
        else:
            pn = op.network.Cubic(shape=[Nx, Ny, Nz], spacing=[ds_ad, ds_ad, ds_ad])
        print('Nx,Ny,Nz', Nx, Ny, Nz)

    if arg.network_type == 'gabriel':
        pts_init = np.random.rand(Npoints, 3) * [Lx_ad, Ly_ad, Lz_ad]
        pts_tmp = []
        if Lz_ad == 0:    # 2D - > adding inlet and outlet nodes
            nnx = int(Nx/2)
            nny = int(Ny/2)
            Lx_adnew = Lx_ad + 1
            Ly_adnew = Ly_ad + 1
            for i in range(0, nnx):    # x direction -> inlet nodes
                pts_tmp.append([i*(Lx_ad/nnx), 0, 0])
            for i in range(0, nny):    # y direction -> inlet nodes
                pts_tmp.append([0, i*(Ly_ad/nny), 0])
            for i in range(0, len(pts_init)):
                pts_tmp.append(pts_init[i])
            for i in range(0, nnx):    # x direction -> outlet nodes
                pts_tmp.append([i*(Lx_ad/nnx), Ly_ad, 0])
            for i in range(0, nny):    # y direction -> outlet nodes
                pts_tmp.append([Lx_ad, i*(Ly_ad/nny), 0])
            pts = np.asarray(pts_tmp)
            pn = op.network.Gabriel(shape=[Lx_adnew, Ly_adnew, Lz_ad], points=pts)
        else:    # 3D - > adding inlet and outlet nodes
            nnx = int(Nx/2)
            nny = int(Ny/2)
            nnz = int(Nz/2)
            Lx_adnew = Lx_ad + 1
            Ly_adnew = Ly_ad + 1
            Lz_adnew = Lz_ad + 1
            for i in range(0, nnx):    # xy direction -> inlet nodes
                for j in range(0, nny):
                    pts_tmp.append([i*(Lx_ad/nnx), j*(Ly_ad/nny), 0])
            for i in range(0, nnx):    # xz direction -> inlet nodes
                for k in range(0, nnz):
                    pts_tmp.append([i*(Lx_ad/nnx), 0, k*(Lz_ad/nnz)])
            for j in range(0, nny):    # yz direction -> inlet nodes
                for k in range(0, nnz):
                    pts_tmp.append([0, j*(Ly_ad/nny), k*(Lz_ad/nnz)])
            for i in range(0, len(pts_init)):
                pts_tmp.append(pts_init[i])
            for i in range(0, nnx):    # xy direction -> outlet nodes
                for j in range(0, nny):
                    pts_tmp.append([i*(Lx_ad/nnx), j*(Ly_ad/nny), Lz_ad])
            for i in range(0, nnx):    # xz direction -> outlet nodes
                for k in range(0, nnz):
                    pts_tmp.append([i*(Lx_ad/nnx), Ly_ad, k*(Lz_ad/nnz)])
            for j in range(0, nny):    # yz direction -> outlet nodes
                for k in range(0, nnz):
                    pts_tmp.append([Lx_ad, j*(Ly_ad/nny), k*(Lz_ad/nnz)])
            pts = np.asarray(pts_tmp)
            pn = op.network.Gabriel(shape=[Lx_adnew, Ly_adnew, Lz_adnew], points=pts)

    Ts_initial = pn['throat.conns']

    print('#############################################################')
    print('Initial Network')

    # Generating a network with the connectivity distribution of rock sample centerlines
    print('#############################################################')
    print('Generating a network with the connectivity distribution of rock sample centerlines')

    for i in range(0, len(pn['pore.coords'])):
        connect_tmp = choices(edgeshist_connectivity[:-1], hist_connectivity)
        Ts_tmp = pn.find_neighbor_throats(pores=[i], flatten=False)
        Ts = []
        for j in range(0, len(Ts_tmp[0])):
            Ts.append(Ts_tmp[0][j])
        np.random.shuffle(Ts)
        niter = len(pn['pore.coords'])
        perc = ((1.0*i)/niter)*100
        result = i % 500
        if result == 0:
            print('progress in % = ', perc)
        if len(Ts) > connect_tmp[0]:
            if connect_tmp[0] > 0:
                dif = int(connect_tmp[0])
                op.topotools.trim(pn, throats=Ts[dif:])

    # Finding nodes with only one link (throad) and adding one more link
    print('#############################################################')
    print('Finding nodes with only one link (throad) and adding one more link')

    if Nz == 1:  # 2D network -> adding one more link to nodes with connective = 1
        for i in range(0, len(pn['pore.coords'])):
            Ps = pn.find_neighbor_pores(pores=i)
            niter = len(pn['pore.coords'])
            perc = ((1.0*i)/niter)*100
            result = i % 500
            if result == 0:
                print('progress in % = ', perc)
            if len(Ps) == 1:
                condition = 0
                for j in range(0, len(Ts_initial)):
                    if Ts_initial[j][0] == i:
                        if (Ts_initial[j][1] != Ps[0] and condition == 0):
                            op.topotools.connect_pores(network=pn,
                                                       pores1=[i],
                                                       pores2=[Ts_initial[j][1]])
                            condition = 1
    else:       # 3D network -> adding one or more links to nodes with connectivity = 1
        for i in range(0, len(pn['pore.coords'])):
            Ps = pn.find_neighbor_pores(pores=i)
            niter = len(pn['pore.coords'])
            perc = ((1.0*i)/niter)*100
            result = i % 500
            if result == 0:
                print('progress in % = ', perc)
            if len(Ps) == 1:
                condition = 1
                connect_tmp = choices(edgeshist_connectivity[:-1], hist_connectivity)
                for j in range(0, len(Ts_initial)):
                    if Ts_initial[j][0] == i:
                        if (Ts_initial[j][1] != Ps[0] and condition < connect_tmp[0]):
                            op.topotools.connect_pores(network=pn,
                                                       pores1=[i],
                                                       pores2=[Ts_initial[j][1]])
                            condition = condition + 1

    fig = op.topotools.plot_connections(network=pn)
    fig = op.topotools.plot_coordinates(network=pn, c='r', s=10, fig=fig)
    plt.savefig(arg.out_folder + '/capillary-network.png')
    plt.clf()

    Net_health = pn.check_network_health()
    print(Net_health)

    # Fixing the network
    if len(Net_health["trim_pores"]) != 0:
        op.topotools.trim(pn, pores=(Net_health["trim_pores"]))
        fig = op.topotools.plot_connections(network=pn)
        fig = op.topotools.plot_coordinates(network=pn, c='r', s=10, fig=fig)
        plt.savefig(arg.out_folder + '/capillary-network-healthy.png')
        plt.clf()
        print('#############################################################')
        Net_health2 = pn.check_network_health()
        print(Net_health2)

    dist = np.zeros(len(pn['throat.conns']), dtype=np.float64)
    r2 = np.zeros(len(pn['throat.conns']), dtype=np.float64)
    for i in range(0, len(pn['throat.conns'])):
        ii = pn['throat.conns'][i][0]
        jj = pn['throat.conns'][i][1]
        x1 = pn['pore.coords'][ii][0]
        y1 = pn['pore.coords'][ii][1]
        z1 = pn['pore.coords'][ii][2]
        x2 = pn['pore.coords'][jj][0]
        y2 = pn['pore.coords'][jj][1]
        z2 = pn['pore.coords'][jj][2]
        dist[i] = np.sqrt((x1-x2)**2 + (y1-y2)**2 + (z1-z2)**2)
        D_tmp = choices(edgeshistD[:-1], histD)
        r2[i] = ((D_tmp[0]/(2*arg.voxel_size*1.0e6)))**2

    xmin_tmp = 100000
    ymin_tmp = 100000
    zmin_tmp = 100000
    for i in range(0, len(pn['pore.coords'])):
        if pn['pore.coords'][i][0] < xmin_tmp:
            xmin_tmp = pn['pore.coords'][i][0]
        if pn['pore.coords'][i][1] < ymin_tmp:
            ymin_tmp = pn['pore.coords'][i][1]
        if pn['pore.coords'][i][2] < zmin_tmp:
            zmin_tmp = pn['pore.coords'][i][2]

    graph_metadata_obj = {'number_of_nodes': pn['pore.coords'].shape[0],
                          'number_of_links': pn['throat.conns'].shape[0]}
    nodes_obj = [
      {
        'id': str(i),
        'metadata': {
            'node_squared_radius': 1,
            'node_coordinates': {
                'x': pn['pore.coords'][i][0] - xmin_tmp,
                'y': pn['pore.coords'][i][1] - ymin_tmp,
                'z': pn['pore.coords'][i][2] - zmin_tmp
            }
        }
      } for i in range(pn['pore.coords'].shape[0])]

    edges_obj = [
      {
        'id': str(i),
        'source': str(pn['throat.conns'][i][0]),
        'target': str(pn['throat.conns'][i][1]),
        'metadata': {
            'link_length': dist[i],
            'link_squared_radius': r2[i]
        }
      } for i in range(pn['throat.conns'].shape[0])]

    graph_obj = {'metadata': graph_metadata_obj,
                 'nodes': nodes_obj,
                 'edges': edges_obj}

    # Build full JSON object
    json_obj = {'graph': graph_obj}

    with open(arg.out_folder + '/new-' + arg.filename, mode='w') as file2:
        json.dump(json_obj, file2, indent=2)

    file2.close()

    # ------------------------------------------------------------------
    #
    #              CAPILLARY NETWORK SECTION ANALYSIS
    #
    # ------------------------------------------------------------------
    # Reading centerlines.json obtained from micro CT scan rock tomography

    with open(arg.out_folder + '/new-' + arg.filename, mode='r') as file3:
        data_pn = json.load(file3, encoding='utf-8')

    # # Extract node geometry arrays from JSON of generated capillary network
    nodes_pn = sorted(data_pn['graph']['nodes'], key=lambda node: int(node['id']))
    x_pn = np.array([node['metadata']['node_coordinates']['x'] for node in nodes_pn], dtype=float)
    y_pn = np.array([node['metadata']['node_coordinates']['y'] for node in nodes_pn], dtype=float)
    z_pn = np.array([node['metadata']['node_coordinates']['z'] for node in nodes_pn], dtype=float)

    # Extract link geometry arrays from JSON of generated capillary network
    edges_pn = sorted(data_pn['graph']['edges'], key=lambda edge: int(edge['id']))
    sources_pn = np.array([int(edge['source']) for edge in edges_pn])
    targets_pn = np.array([int(edge['target']) for edge in edges_pn])
    link_length_pn = np.array([edge['metadata']['link_length'] for edge in edges_pn])
    link_squared_radius_pn = np.array([e['metadata']['link_squared_radius'] for e in edges_pn])

    # Calculating the connectivity of each node of generated capillary network
    connectivity_pn = np.zeros(len(nodes_pn), dtype=np.int32)
    for i in range(0, len(nodes_pn)):
        a_pn = 0
        b_pn = 0
        a_pn = np.sum(sources_pn == i)
        b_pn = np.sum(targets_pn == i)
        connectivity_pn[i] = a_pn + b_pn

    # Comparing the histograms from rock sample and generated capillary network
    # Capillary diameter density of probability distribution
    D_pn = np.sqrt(link_squared_radius_pn) * 2.0 * arg.voxel_size*1e6
    low, high = np.floor(D_pn.min()), np.ceil(D_pn.max())
    binsD_pn = np.linspace(low, high, int(high - low) + 1)
    histD_pn, edgeshistD_pn = np.histogram(D_pn, bins=binsD_pn, density=True)
    plt.hist(D, bins=binsD, density=True, alpha=0.5, label='rock sample')
    plt.hist(D_pn, bins=binsD_pn, density=True, alpha=0.5, label='gen. network')
    plt.legend(loc='upper right')
    plt.xlabel(r'Capillary diameter [$\mu$m]')
    plt.ylabel(r'Probability Density')
    plt.grid()
    plt.savefig(arg.out_folder + '/capillary-diameter.png')
    plt.clf()
    print('Cumulative probability density sum for diameter distribution = ', histD_pn.sum())

    # Capillary length density of probability distribution
    L_pn = link_length_pn*arg.voxel_size*1e6
    low, high = np.floor(L_pn.min()), np.ceil(L_pn.max())
    binsL_pn = np.linspace(low, high, int(high - low) + 1)
    histL_pn, edgeshistL_pn = np.histogram(L_pn, bins=binsL_pn, density=True)
    plt.hist(L, bins=binsL, density=True, alpha=0.5, label='rock sample')
    plt.hist(L_pn, bins=binsL_pn, density=True, alpha=0.5, label='gen. network')
    plt.legend(loc='upper right')
    plt.xlabel(r'Capillary length [$\mu$m]')
    plt.ylabel(r'Probability Density')
    plt.grid()
    plt.savefig(arg.out_folder + '/capillary-length.png')
    plt.clf()
    print('Cumulative probability density sum for length distribution = ', histL_pn.sum())

    # Capillary connectivity density of probability distribution
    low, high = np.floor(connectivity_pn.min()), np.ceil(connectivity_pn.max())
    bins_connectivity_pn = np.linspace(low, high, int(high - low) + 1)
    hist_connectivity_pn, edgeshist_connectivity_pn = np.histogram(connectivity_pn,
                                                                   bins=bins_connectivity_pn,
                                                                   density=True)
    plt.hist(connectivity, bins=bins_connectivity, density=True, alpha=0.5, label='rock sample')
    plt.hist(connectivity_pn, bins=bins_connectivity_pn, density=True, alpha=0.5, label='generated')
    plt.legend(loc='upper right')
    plt.xlabel(r'Capillary connectivity ')
    plt.ylabel(r'Probability Density')
    plt.grid()
    plt.savefig(arg.out_folder + '/capillary-connectivity.png')
    plt.clf()
    print('Cumulative probability density sum for connectivity = ', hist_connectivity_pn.sum())

    # Calculating the porosity
    # Convert capillary dimensions to SI units
    if kk == 1:
        link_length *= arg.voxel_size                                                       # [m]
        link_squared_radius *= arg.voxel_size**2                                            # [m^2]
    link_length_pn *= arg.voxel_size                                                        # [m]
    link_squared_radius_pn *= arg.voxel_size**2                                             # [m^2]

    pore_volume = np.sum(np.pi * link_squared_radius * link_length)                         # [m^3]
    pore_volume_pn = np.sum(np.pi * link_squared_radius_pn * link_length_pn)                # [m^3]
    pore_area_pn = np.sum(np.sqrt(link_squared_radius_pn)*2.0*link_length_pn)               # [m^2]

    xmax = np.amax(x)
    ymax = np.amax(y)
    zmax = np.amax(z)
    xmin = np.amin(x)
    ymin = np.amin(y)
    zmin = np.amin(z)
    V_rock = (xmax-xmin)*(ymax-ymin)*(zmax-zmin)*arg.voxel_size**3                          # [m^3]
    porosity_rock = pore_volume/V_rock

    xmax_pn = np.amax(x_pn)
    ymax_pn = np.amax(y_pn)
    zmax_pn = np.amax(z_pn)
    xmin_pn = np.amin(x_pn)
    ymin_pn = np.amin(y_pn)
    zmin_pn = np.amin(z_pn)
    r2max_pn = np.amax(link_squared_radius_pn)
    Dmax_pn = np.sqrt(r2max_pn)*2.0
    if (zmax_pn-zmin_pn) != 0:
        V_pn = (xmax_pn-xmin_pn)*(ymax_pn-ymin_pn)*(zmax_pn-zmin_pn)*arg.voxel_size**3      # [m^3]
        porosity_pn = pore_volume_pn/V_pn
    else:
        A_pn = (xmax_pn-xmin_pn)*(ymax_pn-ymin_pn) * \
            arg.voxel_size**2                            # [m^2]
        porosity_pn = pore_area_pn/A_pn

    output_file.write(str(porosity_rock)+','+str(porosity_pn)+'\n')

    porosity_diff = porosity_rock - porosity_pn
    if porosity_diff < 0:
        ds_ad = ds_ad + delta_ad
    if porosity_diff > 0:
        ds_ad = ds_ad - delta_ad

    file3.close()
    kk = kk + 1

    print('#############################################################')
    print('Rock sample porosity       = ', porosity_rock)
    print('Capillary network porosity = ', porosity_pn)
    print('#############################################################')
    print('')
    print('                        WARNING')
    print('Porosity or void fraction is a measure of the void (i.e. "empty") spaces')
    print('in a material, and is a fraction of the volume of voids over the total')
    print('volume, between 0 and 1')
    print('')
    print('In case this number is greater than 1, it means that the capillaries')
    print('are overlapping with each other. Try to increase the "capillary_length" parameter')
    print('in the input section')
    print('')
print('program finished')
