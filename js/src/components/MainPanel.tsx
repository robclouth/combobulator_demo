import { Group } from "@mantine/core";
import { observer } from "mobx-react";
import { FC } from "react";
import { Delay } from "./modules/Delay";
import { Latents } from "./modules/Latents";
import { Output } from "./modules/Output";
import { PitchShift } from "./modules/PitchShift";
import { ThreeBandEQ } from "./modules/ThreeBandEQ";

interface Props {}

export const MainPanel: FC<Props> = observer(() => {
  return (
    <Group align="stretch" mah={400}>
      <ThreeBandEQ />
      <PitchShift />
      <Latents />
      <Delay />
      <Output />
    </Group>
  );
});
