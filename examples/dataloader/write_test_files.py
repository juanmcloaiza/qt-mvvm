#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np
import os
import zipfile


def zipdir(path, ziph):
    # ziph is zipfile handle
    for root, dirs, files in os.walk(path):
        for file in files:
            ziph.write(os.path.join(root, file))


def create_if_needed(path):
    if os.path.isdir(path):
        return
    if os.path.isfile(path):
        raise OSError("The specified path is a file")

    try:
        os.makedirs(path)
    except OSError:
        print ("Creation of the directory %s failed" % path)
    else:
        print ("Successfully created the directory %s" % path)
    return


def filepath_to_save_test(dir_name = ".", base_filename = "test_file", suffix = ".out"):
    create_if_needed(dir_name)
    if suffix[0] != ".":
        suffix = "." + suffix
    return os.path.join(dir_name, base_filename + suffix)


def generate_mock_data(nfringes = 1, shift = 0):
    qvals = np.flip(np.linspace(2*np.pi,0,32, endpoint=False))
    qh = (qvals[1] - qvals[0])
    rvals =  qvals**(-4) * (1.01 - np.cos(nfringes * qvals))
    rvals *= (10**shift)/rvals[0]
    dr = 0.4*rvals * (1.1 + np.cos(nfringes * qvals))
    dq = 0.5 * qh + 0.25 * qh * np.sin(nfringes*qvals)
    return qvals, rvals, dr, dq


def write_read_array(filepath, q, r, dr, dq):
    data_array = np.asarray([q, r, dr, dq])
    np.savetxt(filepath, data_array.T)
    print(f"{filepath} generated")
    data_read = np.loadtxt(filepath)
    assert np.all(data_read.T[0] == q)
    assert np.all(data_read.T[1] == r)
    assert np.all(data_read.T[2] == dr)
    assert np.all(data_read.T[3] == dq)


def zip_path(path):
    zipf = zipfile.ZipFile(path+'.zip', 'w', zipfile.ZIP_DEFLATED)
    zipdir(path, zipf)
    zipf.close()


def generate_single_test_files():
    dirpath = f'testfiles/singlefiles'
    for nfringes in [1,2,3,4,5,6,7,8,9,10]:
        filename = "nfringes_"+str(nfringes)
        filepath = filepath_to_save_test(dirpath,filename)
        q, r, dr, dq = generate_mock_data(nfringes)
        write_read_array(filepath, q, r, dr, dq)

        plt.errorbar(q, r, yerr=dr, xerr=dq, ecolor='C0', alpha = 0.1)

    zip_path(dirpath)
    plt.yscale("log")
    plt.show()


def generate_batch_test_files():
    for shift in [1,2,3,4,5,6,7,8,9,10]:
        dirpath = f'testfiles/shift_{shift}'
        for nfringes in [1,2,3,4,5,6,7,8,9,10]:
            filename = "nfringes_"+str(nfringes)
            filepath = filepath_to_save_test(dirpath,filename)
            q, r, dr, dq = generate_mock_data(nfringes, shift)
            write_read_array(filepath, q, r, dr, dq)
            plt.errorbar(q, r, yerr=dr, xerr=dq, color=f'C{shift}', ecolor=f'C{shift}', alpha = 0.1)

        zip_path(dirpath)

    plt.yscale("log")
    plt.show()


if __name__ == '__main__':
    generate_single_test_files()
    generate_batch_test_files()

