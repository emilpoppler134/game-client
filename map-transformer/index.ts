type ColorCode = `${number}.${number}.${number}`;

type TileType = 'water' | 'land';

const map: Record<ColorCode, TileType | undefined> = {
    '0.0.0': 'water',
    '255.255.255': 'land',
    '255.0.0': 'land',
    '0.255.0': 'land',
    '0.0.255': 'land',
};

const rootFile = Bun.file('../src/assets/map/root-map.bmp');

const imageStats = {
    startByte: 54,
    width: 256,
    height: 256,
    pixelByteSize: 3,
    rowBuffer: 4,
};

type ImageStats = typeof imageStats;

const rootDV = new DataView(await rootFile.arrayBuffer());

const targetWidth = imageStats.width / 16;
const targetHeight = imageStats.height / 16;

for (let n = 0; n < 256; n++) {
    console.log(`Chunk: ${n} (${formatName(n)})`);

    const xOffset = (n % 16) * targetWidth;
    const yOffset = Math.floor(n / 16) * targetHeight;

    const targetArray = new Uint8Array(targetWidth * targetHeight);

    for (let y = 0; y < targetWidth; y++) {
        for (let x = 0; x < targetHeight; x++) {
            const tileType = tileTypeAtPixel(
                rootDV,
                imageStats,
                x + xOffset,
                y + yOffset,
                map
            );

            if (tileType === 'water') {
                console.log(`x ${x + xOffset}, y ${y + yOffset}: Empty`);
                targetArray[y * targetWidth + x] = 16;
                continue;
            }

            const tileValue = tileValueNeighboors(
                rootDV,
                imageStats,
                x + xOffset,
                y + yOffset,
                map
            );

            console.log(`x ${x + xOffset}, y ${y + yOffset}: ${tileValue}`);
            targetArray[y * targetWidth + x] = tileValue;
        }
    }

    Bun.write(`../src/assets/map/chunks/${formatName(n)}.chunk`, targetArray);
}

function formatName(n: number) {
    return n.toString().padStart(3, '0');
}

function tileValueNeighboors(
    dataView: DataView,
    s: ImageStats,
    col: number,
    row: number,
    map: Record<ColorCode, TileType | undefined>
): number {
    const north: TileType = tileTypeAtPixel(dataView, s, col, row - 1, map);
    const west: TileType = tileTypeAtPixel(dataView, s, col - 1, row, map);
    const east: TileType = tileTypeAtPixel(dataView, s, col + 1, row, map);
    const south: TileType = tileTypeAtPixel(dataView, s, col, row + 1, map);

    let num = 0;

    if (north === 'land') num += 2 ** 0;
    if (west === 'land') num += 2 ** 1;
    if (east === 'land') num += 2 ** 2;
    if (south === 'land') num += 2 ** 3;

    if (num >= 16) throw new Error(`Value '${num}' is too high`);

    return num;
}

function tileTypeAtPixel(
    dataView: DataView,
    s: ImageStats,
    col: number,
    row: number,
    map: Record<ColorCode, TileType | undefined>
) {
    const colorCode = colorCodeAtPixel(dataView, s, col, row);

    const tileValue = colorCode === 'outside' ? 'land' : map[colorCode];

    if (tileValue === undefined) {
        throw new Error(`Undefined key: '${colorCode}'`);
    }

    return tileValue;
}

function colorCodeAtPixel(
    dataView: DataView,
    s: ImageStats,
    col: number,
    row: number
): ColorCode | 'outside' {
    if (col < 0 || col >= s.width || row < 0 || row >= s.height) {
        return 'outside';
    }

    const rowSize =
        Math.ceil((s.width * s.pixelByteSize) / s.rowBuffer) * s.rowBuffer;
    const currentOffset =
        s.startByte + (s.height - 1 - row) * rowSize + col * s.pixelByteSize;

    const r = dataView.getUint8(currentOffset + 2);
    const g = dataView.getUint8(currentOffset + 1);
    const b = dataView.getUint8(currentOffset + 0);

    return `${r}.${g}.${b}`;
}
