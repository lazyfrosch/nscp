package { "git": ensure => present }
package { "build-essential": ensure => present }
package { "cmake": ensure => present }
package { "python-dev": ensure => present }
package { "libssl-dev": ensure => present }
package { "libboost-all-dev": ensure => present }
package { "libprotobuf-dev": ensure => present }
package { "protobuf-compiler": ensure => present }
package { "python-protobuf": ensure => present }
package { "rst2pdf": ensure => present }
package { "python-sphinx": ensure => present }
package { "libcrypto++-dev": ensure => present }
package { "liblua5.1-0-dev": ensure => present }
package { "libgtest-dev": ensure => present }

file { "/usr/share/pyshared/google/protobuf/compiler/__init__.py": 
	ensure    => present, 
	subscribe => Package['python-protobuf']
}
file { "/usr/lib/python2.7/dist-packages/google/protobuf/compiler/__init__.py": 
	ensure    => present,
	subscribe => Package['python-protobuf']
}
file { "/home/vagrant/build.sh":
    ensure    => "present",
    mode      => 755,
	source    => "/etc/puppet/files/build.sh",
}


exec { "apt-update":
    command => "/usr/bin/apt-get update"
}

Exec["apt-update"] -> Package <| |>