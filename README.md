# Prayer CLI Tool

The tool works by fetching the prayer times from the public API: [aladhan.com](https://aladhan.com/prayer-times-api) and prints them to the terminal in a formatted way.

### Usage

```bash
prayer <countryCode> <cityName>
```

An example use case, along with the printed output, is the following:

```bash
prayer US New-York
```

Output:

```
✦───────────────────────────────────────✦
             Prayer Times
        US, New-York · 16 Sep 2026
✦───────────────────────────────────────✦

           Fajr          05:22
           Sunrise       06:38
           Dhuhr         12:51
           Asr           16:22
           Maghrib       19:03
           Isha          20:19
           Last Third    02:46

  Upcoming: Lastthird  Reamining: 04:45

✦───────────────────────────────────────✦
```

> **Note:** the input is case insensitive. The only condition is that city names consisting of more than one word must be hyphenated (as shown in the example above, `New-York`).

### Dependencies

| Dependency | Version |
| ---------- | ------- |
| make       | ^4.4.1  |
| git        | ^2.55.0 |
| curl       | ^8.22.0 |
| cjson      | ^1.7.19 |
| gcc        | ^16.2.1 |

All five dependencies exist on all major package managers (apt, pacman, etc.) and can be installed with a single command.

Example for pacman:

```bash
sudo pacman -S git make curl cjson gcc
```

### Installation

```bash
git clone https://github.com/KTD-11/Prayer-Times-CLI-tool.git
cd ./Prayer-Times-CLI-tool
make
sudo make install
```

That's pretty much it!

### License

This project is licensed under the GNU General Public License v3.0. see [LICENSE](LICENSE) for details.
