from setuptools import setup

setup(name='splatgl',
      version='0.0.0',
      description='Splat Graphics Library',
      long_description='An SDL2 and OpenGL-based 2D tile/sprite renderer',
      url='https://digitalbytes.net/projects/splatgl',
      author='Michael Dale Long',
      author_email='mlong@digitalbytes.net',
      license='zlib',
      packages=['splatgl'],
      zip_safe=False,
      classifiers=[
          'Development Status :: 3 - Alpha',
          'License :: OSI Approved :: zlib/libpng License',
          'Programming Language :: Python :: 2.7',
          'Intended Audience :: Developers',
          'Topic :: Multimedia :: Graphics',
          ],
      keywords='tile sprite renderer game graphics',
      install_requires=[
          'PySDL2',
          'enum34',
      ],
      include_package_data=True)

